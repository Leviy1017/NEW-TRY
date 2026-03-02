#include <stdio.h>
#include <stdlib.h>

int main() {
    int array[5] = {1, 2, 3, 4, 5};

    printf("合法访问:array[4] = %d\n", array[4]);

    printf("越界读取:array[5] = %d\n", array[5]);

    array[6] = 42;
    printf("越界写入:array[6] = %d\n", array[6]);

    printf("更远的越界访问:array[10] = %d\n", array[10]);

    return 0;
}