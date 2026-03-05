#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DATA_COUNT 10000    // 生成的数据总量
#define OUTPUT_TO_FILE 1    // 1:输出到文件 0:输出到控制台
#define FILE_NAME "random_data_txt"
#define TIMESTAMP_TYPE 2
#define CONTENT_LEN 32    // 内容字符串固定长度32字节

// 环形缓冲区大小（建议设置为2的幂，便于取模运算）
#define RING_BUFFER_SIZE 1024

// 定义数据节点结构
typedef struct {
    int id;
    long long timestamp;
    char content[CONTENT_LEN + 1];
    int valid;  // 标记该节点是否有效(1:有效 0:无效)
} DataNode;

// 环形缓冲区结构
typedef struct {
    DataNode buffer[RING_BUFFER_SIZE];
    int head;  // 读指针
    int tail;  // 写指针
    int count; // 当前缓冲区中的数据量
} RingBuffer;

// 初始化环形缓冲区
void init_ring_buffer(RingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    memset(rb->buffer, 0, sizeof(rb->buffer));
}

// 向环形缓冲区写入数据
int ring_buffer_write(RingBuffer *rb, int id, long long timestamp, char *content) {
    if (rb->count >= RING_BUFFER_SIZE) {
        return 0; // 缓冲区已满
    }
    
    rb->buffer[rb->tail].id = id;
    rb->buffer[rb->tail].timestamp = timestamp;
    strcpy(rb->buffer[rb->tail].content, content);
    rb->buffer[rb->tail].valid = 1;
    
    rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;
    rb->count++;
    return 1;
}

// 从环形缓冲区读取数据
int ring_buffer_read(RingBuffer *rb, DataNode *node) {
    if (rb->count <= 0) {
        return 0; // 缓冲区为空
    }
    
    *node = rb->buffer[rb->head];
    rb->buffer[rb->head].valid = 0; // 标记为无效
    
    rb->head = (rb->head + 1) % RING_BUFFER_SIZE;//取余数，假设有108个，下一个正好形成环形
    rb->count--;
    return 1;
}

// 获取缓冲区中的数据量
int ring_buffer_count(RingBuffer *rb) {
    return rb->count;
}
//见production代码
int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

void generate_32bytes_content(char *content) {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int charset_len = strlen(charset);

    for (int i = 0; i < CONTENT_LEN; i++) {
        content[i] = charset[random_int(0, charset_len - 1)];
    }
    content[CONTENT_LEN] = '\0';
}

long long generate_timestamp() {
    if (TIMESTAMP_TYPE == 1) {
        // 秒级时间戳：10位数字
        return (long long)time(NULL);
    } else {
        // 毫秒级时间戳：13位数字
        struct timespec ts;
        // 使用单调时钟，不受系统时间修改影响，兼容性更好
        //它的核心优势是「不被系统时间修改影响」，适合超时控制、性能统计等场景
        clock_gettime(CLOCK_MONOTONIC, &ts);  
        // 秒转毫秒 + 纳秒转毫秒（tv_nsec单位是纳秒，除以1e6得到毫秒）
        return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    }
}

// 简单的重排函数：按照一定顺序读取缓冲区中的数据
void reorder_and_output(RingBuffer *rb, FILE *fp) {
    // 最简单的重排：每隔3个位置读取一个数据
    // 这样可以打乱原始顺序
    DataNode temp_buffer[RING_BUFFER_SIZE];//temp临时缓冲区 保证装下所有数据
    int temp_count = 0;
    
    // 先把所有数据暂存到临时数组
    while (ring_buffer_count(rb) > 0) {
        ring_buffer_read(rb, &temp_buffer[temp_count]);
        temp_count++;
    }
    
    // 按照新的顺序输出
    // 逆序输出
    for (int i = temp_count - 1; i >= 0; i--) {
        if (OUTPUT_TO_FILE) {
            fprintf(fp, "%d %lld %s\n", //输出到文件
                    temp_buffer[i].id, 
                    temp_buffer[i].timestamp, 
                    temp_buffer[i].content);
        } else {
            printf("%d %lld %s\n", //输出到控制台
                   temp_buffer[i].id, 
                   temp_buffer[i].timestamp, 
                   temp_buffer[i].content);
        }
    }
    
  
}

int main() {
    srand((unsigned int)time(NULL));

    FILE *fp = NULL;
    if (OUTPUT_TO_FILE) {          //如果打开文件
        fp = fopen(FILE_NAME, "w");
        if (fp == NULL) {
            printf("文件打开失败！\n");
            return 1;
        }
    }

    // 初始化环形缓冲区
    RingBuffer rb;
    init_ring_buffer(&rb);
    
    int batch_size = 100; // 每生成100条数据就进行一次重排输出
    
    printf("开始生成数据并使用环形缓冲区进行重排...\n");
    
    for (int i = 1; i <= DATA_COUNT; i++) {
        char content[CONTENT_LEN + 1];
        long long timestamp = generate_timestamp();
        
        generate_32bytes_content(content);//进行数据生成
        
        // 写入环形缓冲区
        if (!ring_buffer_write(&rb, i, timestamp, content)) {
            printf("环形缓冲区已满，进行重排输出...\n");
            // 如果缓冲区满了，先重排输出当前缓冲区的数据
            reorder_and_output(&rb, fp);
            // 重新写入当前数据
            ring_buffer_write(&rb, i, timestamp, content);
        }
        
        // 每生成batch_size条数据，进行一次重排输出
        if (i % batch_size == 0 && ring_buffer_count(&rb) >= batch_size / 2) {
            printf("生成 %d 条数据，进行重排输出...\n", i);
            reorder_and_output(&rb, fp);
        }
    }
    
    // 最后将缓冲区中剩余的数据重排输出
    if (ring_buffer_count(&rb) > 0) {
        printf("输出剩余 %d 条数据...\n", ring_buffer_count(&rb));
        reorder_and_output(&rb, fp);
    }

    if (OUTPUT_TO_FILE) {
        fclose(fp);
        printf("数据生成完成！共生成 %d 条数据，已保存到 %s\n", DATA_COUNT, FILE_NAME);
    }

    return 0;
}