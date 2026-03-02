#include <stdio.h>
#include <stdlib.h>

struct queue {
    int* data;
    int front;
    int rear;
    int maxCapacity;
};

void initqueue(struct queue* q, int capacity) {
    q->maxCapacity = capacity + 1;
    q->data = (int*)malloc(q->maxCapacity * sizeof(int)); // 修正：应该是int*而不是struct queue*
    q->front = 0;
    q->rear = 0;
}

int isEmpty(struct queue* q) {
    return q->front == q->rear; // 修正：队列为空的条件是front等于rear
}

int isFull(struct queue* q) {
    return (q->rear + 1) % q->maxCapacity == q->front; // 修正：添加括号确保正确的运算顺序
}

int enqueue(struct queue* q, int data) {
    if (isFull(q)) {
        return 0;
    }
    q->data[q->rear] = data;
    q->rear = (q->rear + 1) % q->maxCapacity; // 修正：应该是模maxCapacity而不是rear
    return 1;
}

int dequeue(struct queue* q, int* data) {
    if (isEmpty(q)) {
        return 0;
    }
    *data = q->data[q->front];
    q->front = (q->front + 1) % q->maxCapacity;
    return 1;
}

int getFront(struct queue* q, int* data) {
    if (isEmpty(q)) {
        return 0;
    }
    *data = q->data[q->front];
    return 1;
}

void destroyqueue(struct queue* q) {
    free(q->data);
    q->data = NULL;
    q->front = q->rear = 0;
    q->maxCapacity = 0;
}

int main() {
    struct queue q;
    int data;

    initqueue(&q, 5);
    printf("初始化队列,容量为5\n");

    printf("入队操作：");
    for (int i = 1; i <= 6; i++) {
        if (enqueue(&q, i)) {
            printf("%d ", i);
        } else {
            printf("\n入队 %d 失败，队列已满\n", i);
        }
    }

    if (getFront(&q, &data)) {
        printf("\n当前队头元素: %d\n", data);
    }
    
    printf("出队操作：");
    while (dequeue(&q, &data)) {
        printf("%d ", data);
    }
    printf("\n");

    printf("队列是否为空: %s\n", isEmpty(&q) ? "是" : "否");
     
    printf("再次入队三个元素:10, 20, 30\n");
    enqueue(&q, 10);
    enqueue(&q, 20);
    enqueue(&q, 30);

    if (getFront(&q, &data)) {
        printf("当前队头元素: %d\n", data);
    }

    if (dequeue(&q, &data)) {
        printf("出队元素： %d\n", data);
    }

    if (getFront(&q, &data)) {
        printf("出队后队头元素：%d\n", data);
    }

    destroyqueue(&q);
    return 0;
}





