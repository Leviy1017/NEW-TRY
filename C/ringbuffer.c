#include<stdio.h>
#include <stdlib.h> // 提供 malloc、free 函数
#include <stdbool.h> // 提供 bool、true、false 类型
#include <stdint.h>//

typedef struct{
    uint8_t* buffer;
    size_t head;
    size_t tail;
    size_t size;                                   
    bool full;
}ringbuffer_t;

void ring_buffer_init(ringbuffer_t* rb, size_t size){
    rb->buffer = (uint8_t *)malloc(size);//TODO 环形缓冲区一定是一个固定大小的buffer
    rb->head = 0;
    rb->tail = 0;
    rb->size = size;
    rb->full = false;
}


bool ringbuffer_is_full(const ringbuffer_t *rb){
    return rb->full;
}

bool ringbuffer_is_empty(const ringbuffer_t *rb){
    return (!rb->full && (rb->head == rb->tail));//头指针和尾指针指向同一位置且函数不满才是空，不然还有满的情况下头尾指针指相同位置
    //TODO 怎么做环形缓冲区的边界检查，满的情况怎么处理避免丢失数据，怎样高效的处理数据，避免无意义的拷贝
}

size_t ringbuffer_available_data(const ringbuffer_t *rb){
    if(rb->full){
        return rb->size;
    }
    return(rb->head >= rb->tail) ? (rb->head - rb->tail) : (rb->size - rb->tail + rb->head);
}

size_t ringbuffer_available_space(const ringbuffer_t *rb){
    return rb->size - ringbuffer_available_data(rb);
}

void ringbuffer_write(ringbuffer_t *rb, const uint8_t *data, size_t len){
    if(len > ringbuffer_available_space(rb)){
        return;
    }

    for(size_t i = 0; i < len; i++){
        rb->buffer[rb->head] = data[i];
        rb->head = (rb->head + 1) % rb->size;
        //rb->head + 1：写指针向前移动一位
        //rb->size：取模运算，实现"环形"
        //rb->head =：更新写指针
    }

    if(len > 0){
        rb->full = (rb->head == rb->tail);
        //写入后判断是否写满
    }
}

void ringbuffer_read(ringbuffer_t *rb, uint8_t *data, size_t len){
    if(len > ringbuffer_available_data(rb)){
    //如果写入数据超出可用空间时
    return;      
    // void函数中return表示提前结束函数，void函数不返回任何值
    }
    
    for (size_t i = 0; i < len; i++){
        data[i] = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) % rb->size;
        //同上，实现循环的设计
    }

    if(len > 0){
        rb->full = false;
    }
}

void ringbuffer_free(ringbuffer_t *rb){
    free(rb->buffer);
}


//TODO ringbuffer对比普通的queue有什么优势
//TODO 1. 处理多线程环境下的数据问题（mutex  atomic）
//TODO 2. 模拟多生产者多消费者模型，写成第一个项目
