#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>  // 对于GCC/Clang

// 方法1a：内联汇编读取时间戳计数器
static inline uint64_t rdtsc() {
    unsigned int lo, hi;
    __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

// 方法1b：使用编译器内置函数（GCC/Clang）
static inline uint64_t rdtsc_intrinsic() {
    return __rdtsc();
}

int main() {
    uint64_t start, end, cycles;
    
    // 防止指令重排序
    __asm__ volatile ("mfence");
    
    start = rdtsc();
    
    // 要测量的代码段
    volatile int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += i;
    }
    
    __asm__ volatile ("mfence");
    end = rdtsc();
    
    cycles = end - start;
    printf("时钟周期数: %lu\n", cycles);
    printf("大约耗时: %.2f ns (假设3.0GHz CPU)\n", 
           (double)cycles / 3.0);
    
    return 0;
}