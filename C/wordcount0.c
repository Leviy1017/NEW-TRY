#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdatomic.h>
#include <ctype.h>
#include <time.h>

#define NUM_THREADS 6
#define READ_SIZE 1048576
#define LOOKAHEAD_SIZE 1024
#define MAX_THREADS 8

typedef struct {
    int fd;
    off_t offset;
    size_t length;
    atomic_int word_count;
} ThreadData;

void *count_words(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int fd = data->fd;
    off_t offset = data->offset;
    size_t length = data->length;
    int count = 0;
    int in_word = 0;

    size_t buffer_size = READ_SIZE + LOOKAHEAD_SIZE;
    char *buffer = (char *)malloc(buffer_size);
    if (buffer == NULL) {
        perror("Error allocating memory");
        return NULL;
    }

    while (length > 0) {
        size_t to_read = (length > READ_SIZE) ? READ_SIZE : length;
        
        ssize_t read_size = pread(fd, buffer, to_read, offset);
        if (read_size < 0) {
            perror("Error reading file");
            free(buffer);
            return NULL;
        }

        if (read_size == 0) break;

        // 处理边界情况
        if (length > READ_SIZE && read_size == to_read) {
            ssize_t extra_read = pread(fd, buffer + read_size, LOOKAHEAD_SIZE, offset + read_size);
            if (extra_read > 0) {
                read_size += extra_read;
            }
        }

        // 确保不在单词中间结束
        if (length > READ_SIZE && read_size > 0) {
            ssize_t i = read_size - 1;
            while (i > 0 && !isspace(buffer[i])) {
                i--;
            }
            read_size = i + 1;
        }

        // 统计单词
        for (ssize_t i = 0; i < read_size; i++) {
            if (isspace(buffer[i])) {
                if (in_word) {
                    count++;
                    in_word = 0;
                }
            } else {
                in_word = 1;
            }
        }

        offset += read_size;
        if (read_size > length) break;
        length -= read_size;
    }

    if (in_word) {
        count++;
    }

    data->word_count = count;
    free(buffer);
    return NULL;
}

int main() {
    struct timespec start, end;
    int64_t cpu_time_used;
    
    // 正确的错误检查方式
    int ret = clock_gettime(CLOCK_MONOTONIC, &start);
    if (ret != 0) {
        perror("Error getting start time");
        return 1;
    }
    
    printf("Opening file...\n");
    int fd = open("english.txt", O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return 1;
    }

    printf("File opened successfully.\n");

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("Error getting file size");
        close(fd);
        return 1;
    }
    off_t file_size = st.st_size;
    printf("File size: %ld bytes\n", (long)file_size);

    if (file_size == 0) {
        printf("File is empty\n");
        close(fd);
        return 0;
    }

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
   
    size_t chunk_size = file_size / NUM_THREADS;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("Creating thread %d...\n", i);
        thread_data[i].fd = fd;
        thread_data[i].offset = i * chunk_size;
        thread_data[i].length = (i == NUM_THREADS - 1) ? (file_size - i * chunk_size) : chunk_size;
        thread_data[i].word_count = 0;
        
        ret = pthread_create(&threads[i], NULL, count_words, &thread_data[i]);
        if (ret != 0) {
            fprintf(stderr, "Error creating thread: %s\n", strerror(ret));
            close(fd);
            return 1;
        }
        printf("Thread %d created successfully.\n", i);
    }

    int total_word_count = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("Joining thread %d...\n", i);
        pthread_join(threads[i], NULL);
        printf("Thread %d joined. Word count: %d\n", i, thread_data[i].word_count);
        total_word_count += thread_data[i].word_count;
    }

    close(fd);

    printf("Total word count: %d\n", total_word_count);

    ret = clock_gettime(CLOCK_MONOTONIC, &end);
    if (ret != 0) {
        perror("Error getting end time");
        return 1;
    }
    
    // 当前的时间计算有误，应该这样计算：
    cpu_time_used = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
    //end.tv_sec - start.tv_sec：计算秒数的差值
    //end.tv_nsec - start.tv_nsec：计算纳秒数的差值
    // 1000000000.0：将秒转换为纳秒（1秒 = 10⁹纳秒）
    //两部分相加得到总的时间（秒）
    printf("Time used: %ld \n", cpu_time_used);

    return 0;
    //时间轴：0秒 ---> start时间点 ---> 程序执行 ---> end时间点 ---> 现在
    ///测量过程：
     //1. 在程序开始处记录 start 时间点
    //2. 程序执行各种操作（文件I/O、多线程计算等）
    //3. 在程序结束处记录 end 时间点  
    //4. 计算 end - start 得到程序运行总时间
}