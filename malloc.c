//malloc之后，必须要free释放
//不释放就可能发生内存泄漏
//c语言没有gc机制，所以依靠程序员手动管理内存，门槛很高
//有gc机制的语言如：go、java...
//虽然c语言没有gc机制，但是他的上限是比有gc的语言高
//所以c的极致性能比任何语言都要高
//你以后要学的是go，java不用学，因为国内java已死，上限不高
//go的性能比java好，语法简答，多新兴企业在用，比如momenta、字节、阿里...

//既然malloc不能无限申请，或者说malloc申请多少内存会出现性能暴跌
//所以我们需要探究malloc函数是怎么实现的
//内核是如何调用malloc
//可以写一个benchmark（性能测试）

//测试特定大小内存块申请释放的速度
//第一个参数是申请内存块的大小，第二个参数是测试的次数
//返回值是总申请时间的耗时，单位是ms

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define START_SIZE 1000000     // 第一次测试的内存块大小
#define STEP       1000000     // 每次增加的步长
#define MAX_SIZE   40000000    // 最大测试内存块大小
#define COUNT      1000000    // 每轮测试的 msalloc/free 次数    
int benchmark_mem(int size, int count) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < count; ++i) {
        void *p = malloc(size);
        free(p);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 +
                      (end.tv_nsec - start.tv_nsec) / 1000000;
    return (int)elapsed_ms;
}

int main(void) {
    int size = START_SIZE;
    for (int nth = 1; size <= MAX_SIZE; ++nth) {
        int ms = benchmark_mem(size, COUNT);

        if (nth == 1)       printf("第一次");
        else if (nth == 2)  printf("第二次");
        else                printf("第%d次", nth);

        printf("size: %d bytes, Time: %d ms\n", size, ms);
        size += STEP;
    }
    return 0;
}

/*
    malloc在32mb这个边界以前，是通过brk()/sbrk()系统调用，来申请堆(heap)，他不是真正的去内核申请了一块内存地址，
    是在堆这个buffer进行申请释放，所以很快
    **这里贴一个perf函数调用栈malloc的图片
    那超过32mb这个边界，就通过mmap()系统调用，去申请内存
    那是不是我们用户态申请要做一次上下文切换，进入内核态
    用户态和内核态本质是权限组
    mmap申请内存的是怎么做的
    tlb没有严格的用户态和内核态区分，它既可以再用户态操作，也可以在内核态操作
    首先，用户态会访问tlb，如果tlb里没有对应的地址，发生tlb miss，然后进入内核态，访问page table
    所以tlb是page的缓存
    然后page返回一段地址给tlb，也就发生了flush_tlb,mmap就成功申请到了内存地址
    所以这就是慢的本质原因

    page
    任何修改页表的操作都会触发一次page fault（缺页中断）
    为什么这样设计
    因为我们要保证cpu的每个核心看的数据要一致
    cpu1核修改了一张页表，如果不触发page fault，修改页表上的信息
    那么在这个时间段里，cpu2核...访问了这张被修改的页表，
    那么cpu2核是不是读到了旧数据，也就是脏数据
    因为他看到不是最新的

    page 4k 对齐 - 4x1024=4096
    4k在计算机发展历史中，是一个正正好好的值
    大了遍历就慢了，小了存的数据少，是不是还要跨页访问，又慢了
    所以我们在申请内存的时候，最好申请1024的整数倍
    假如我们申请了4097
    是不是跨页了
    多出来的1个字节存到另一张页表上
    现在总共申请了4k+4k两张页表
    但是实际用到的只有4k + 1byte
    4095就变成了anon匿名内存，没有实际使用，但是申请出来了

*/