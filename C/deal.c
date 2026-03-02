#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>  // 线程库
#include <unistd.h>   // 用于usleep模拟延时

/************************ 公共配置（与原有代码兼容） ************************/
#define DATA_COUNT 10000        // 数据总量
#define CONTENT_LEN 32          // 内容固定32字节
#define REMAINDER_DIVISOR 10    // ID余数除数（按ID%10分组）
#define MAX_LINE_LEN 100        // 每行数据最大长度

/************************ 环形缓冲区（Ring Buffer）定义 ************************/
#define RING_BUFFER_SIZE 1024   // 环形缓冲区大小（建议为2的幂，也可自定义，需大于单次生产/消费数据量）

// 数据项结构（与原有数据格式一致）
typedef struct {
    int id;                     // 数据ID
    long long timestamp;        // 时间戳
    char content[CONTENT_LEN + 1]; // 32字节内容+结束符
} DataItem;

// 环形缓冲区结构
typedef struct {
    DataItem buffer[RING_BUFFER_SIZE]; // 缓冲区存储数组
    int head;                      // 头指针：生产者写入位置（下一个可写入索引）
    int tail;                      // 尾指针：消费者读取位置（下一个可读取索引）
    int count;                     // 当前缓冲区有效数据个数
    pthread_mutex_t mutex;         // 互斥锁：保护缓冲区读写安全
    pthread_cond_t not_full;       // 条件变量：缓冲区非满（生产者等待）
    pthread_cond_t not_empty;      // 条件变量：缓冲区非空（消费者等待）
} RingBuffer;

/************************ 环形缓冲区操作函数 ************************/
// 初始化环形缓冲区
void ring_buffer_init(RingBuffer *rb) {
    if (rb == NULL) {
        return;
    }
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    pthread_mutex_init(&rb->mutex, NULL);
    pthread_cond_init(&rb->not_full, NULL);
    pthread_cond_init(&rb->not_empty, NULL);
}

// 向环形缓冲区写入数据（生产者调用，阻塞式）
int ring_buffer_write(RingBuffer *rb, const DataItem *item) {
    if (rb == NULL || item == NULL) {
        return -1; // 参数无效
    }

    // 加互斥锁，独占缓冲区
    pthread_mutex_lock(&rb->mutex);

    // 等待缓冲区非满（如果缓冲区满了，生产者阻塞）
    while (rb->count >= RING_BUFFER_SIZE) {
        pthread_cond_wait(&rb->not_full, &rb->mutex);
    }

    // 写入数据到缓冲区
    rb->buffer[rb->head] = *item;
    // 更新头指针（环形特性：取模实现循环）
    rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
    rb->count++;

    // 唤醒等待缓冲区非空的消费者
    pthread_cond_signal(&rb->not_empty);

    // 释放互斥锁
    pthread_mutex_unlock(&rb->mutex);

    return 0; // 写入成功
}

// 从环形缓冲区读取数据（消费者调用，阻塞式）
int ring_buffer_read(RingBuffer *rb, DataItem *item) {
    if (rb == NULL || item == NULL) {
        return -1; // 参数无效
    }

    // 加互斥锁，独占缓冲区
    pthread_mutex_lock(&rb->mutex);

    // 等待缓冲区非空（如果缓冲区空了，消费者阻塞）
    while (rb->count <= 0) {
        pthread_cond_wait(&rb->not_empty, &rb->mutex);
    }

    // 读取缓冲区数据
    *item = rb->buffer[rb->tail];
    // 更新尾指针（环形特性：取模实现循环）
    rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;
    rb->count--;

    // 唤醒等待缓冲区非满的生产者
    pthread_cond_signal(&rb->not_full);

    // 释放互斥锁
    pthread_mutex_unlock(&rb->mutex);

    return 0; // 读取成功
}

// 销毁环形缓冲区（释放资源）
void ring_buffer_destroy(RingBuffer *rb) {
    if (rb == NULL) {
        return;
    }
    pthread_mutex_destroy(&rb->mutex);
    pthread_cond_destroy(&rb->not_full);
    pthread_cond_destroy(&rb->not_empty);
    memset(rb, 0, sizeof(RingBuffer));
}

/************************ 原有工具函数（兼容复用） ************************/
// 生成指定区间随机整数
int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

// 生成32字节随机内容
void generate_32bytes_content(char *content) {
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int charset_len = strlen(charset);
    for (int i = 0; i < CONTENT_LEN; i++) {
        content[i] = charset[random_int(0, charset_len - 1)];
    }
    content[CONTENT_LEN] = '\0'; // 修正原有代码的 '/' 错误
}

// 生成时间戳（毫秒级）
long long generate_timestamp() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts); // 修正原有代码的函数参数错误
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/************************ 生产者线程函数 ************************/
void *producer_thread(void *arg) {
    RingBuffer *rb = (RingBuffer *)arg;
    srand((unsigned int)time(NULL) + pthread_self()); // 线程独立随机种子

    for (int i = 1; i <= DATA_COUNT; i++) {
        // 构造数据项
        DataItem item;
        item.id = i;
        item.timestamp = generate_timestamp();
        generate_32bytes_content(item.content);

        // 写入环形缓冲区
        ring_buffer_write(rb, &item);

        // 每1000条打印进度
        if (i % 1000 == 0) {
            printf("生产者：已生成并写入 %d 条数据\n", i);
        }

        // 可选：轻微延时，模拟实际生产速度
        usleep(100);
    }

    printf("生产者：数据生产完成，共写入 %d 条数据\n", DATA_COUNT);
    pthread_exit(NULL);
    return NULL;
}

/************************ 消费者线程函数（按ID余数重排） ************************/
void *consumer_thread(void *arg) {
    RingBuffer *rb = (RingBuffer *)arg;
    // 存储按余数分组的数据
    DataItem sorted_data[REMAINDER_DIVISOR][DATA_COUNT];
    int group_count[REMAINDER_DIVISOR] = {0}; // 每组数据计数
    int read_count = 0; // 已读取数据总数

    // 持续读取，直到读取完所有数据
    while (read_count < DATA_COUNT) {
        DataItem item;
        // 从环形缓冲区读取数据
        if (ring_buffer_read(rb, &item) == 0) {
            // 计算ID余数，确定分组
            int remainder = item.id % REMAINDER_DIVISOR;
            if (remainder < 0) {
                remainder += REMAINDER_DIVISOR;
            }

            // 存入对应分组
            sorted_data[remainder][group_count[remainder]] = item;
            group_count[remainder]++;
            read_count++;

            // 每1000条打印进度
            if (read_count % 1000 == 0) {
                printf("消费者：已读取 %d 条数据\n", read_count);
            }
        }

        // 可选：轻微延时，模拟实际消费速度
        usleep(100);
    }

    // 打开输出文件，存储重排后的数据
    FILE *out_fp = fopen("sorted_data_by_ringbuffer.txt", "w");
    if (out_fp == NULL) {
        perror("消费者：打开输出文件失败");
        pthread_exit(NULL);
        return NULL;
    }

    // 对每个余数分组内按ID排序，并写入文件
    printf("消费者：开始按ID余数重排数据...\n");
    for (int rem = 0; rem < REMAINDER_DIVISOR; rem++) {
        if (group_count[rem] == 0) {
            continue;
        }

        // 冒泡排序：分组内按ID从小到大排序
        for (int i = 0; i < group_count[rem] - 1; i++) {
            for (int j = 0; j < group_count[rem] - 1 - i; j++) {
                if (sorted_data[rem][j].id > sorted_data[rem][j+1].id) {
                    DataItem temp = sorted_data[rem][j];
                    sorted_data[rem][j] = sorted_data[rem][j+1];
                    sorted_data[rem][j+1] = temp;
                }
            }
        }

        // 写入分组标识
        fprintf(out_fp, "===== ID %% %d = %d (共 %d 条数据) =====\n",
                REMAINDER_DIVISOR, rem, group_count[rem]);
        // 写入分组内数据
        for (int i = 0; i < group_count[rem]; i++) {
            DataItem *data = &sorted_data[rem][i];
            fprintf(out_fp, "%d %lld %s\n", data->id, data->timestamp, data->content);
        }
        fprintf(out_fp, "\n");
    }

    // 关闭文件
    fclose(out_fp);

    // 打印完成信息
    printf("消费者：数据重排完成！\n");
    printf("消费者：共读取 %d 条有效数据\n", read_count);
    printf("消费者：重排后数据保存到 sorted_data_by_ringbuffer.txt\n");
    printf("消费者：各余数分组统计：\n");
    for (int rem = 0; rem < REMAINDER_DIVISOR; rem++) {
        printf("  余数 %d：%d 条数据\n", rem, group_count[rem]);
    }

    pthread_exit(NULL);
    return NULL;
}

/************************ 主函数（线程创建与管理） ************************/
int main() {
    // 1. 初始化环形缓冲区
    RingBuffer rb;
    ring_buffer_init(&rb);

    // 2. 创建生产者线程和消费者线程
    pthread_t producer_tid;
    pthread_t consumer_tid;
    int ret1 = pthread_create(&producer_tid, NULL, producer_thread, &rb);
    int ret2 = pthread_create(&consumer_tid, NULL, consumer_thread, &rb);

    if (ret1 != 0 || ret2 != 0) {
        perror("创建线程失败");
        ring_buffer_destroy(&rb);
        return 1;
    }

    // 3. 等待线程执行完成
    pthread_join(producer_tid, NULL);
    pthread_join(consumer_tid, NULL);

    // 4. 销毁环形缓冲区
    ring_buffer_destroy(&rb);

    printf("主程序：生产者-消费者流程全部完成\n");
    return 0;
}