/*编写一个程序，完成以下功能：

定义一个包含10个整数的数组，并初始化数组元素为1,3,5,7,9,11,13,15,17,19

使用指针遍历数组，计算所有元素的平均值

使用指针找出数组中的最大值和最小值

使用指针将数组元素逆序排列

打印原始数组、逆序后的数组、平均值、最大值和最小值*/

#include<stdio.h>
int main(){
    int nums[10] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
    //使用指针指向数组首地址
    int *p = nums;
    int avg = 0;
    int sum = 0;
    for(int i = 0; i < 10; i++) {
        sum = sum + *p;
        p++;
    }
    avg = sum / 10;
    int MAX = 0;
    for(int j = 0; j < 10; j++) {
        if (j < MAX) {
            return MAX;
        } else {
            return j;
        }
    }
    int min = 0;
    for(int a = 0; a < 10; a++) {
        if (a > min) {
            return min;
        } else {
            return a;
        }
    }

    int temp = 0;
    int size = sizeof(nums)/ sizeof(nums[0]);
    for(int b = 0; b < size; b++) {
        if(nums[b] < nums[b + 1]) {
            temp = nums [b + 1];
            nums [b + 1] = nums [b];
            nums [b] = temp;
            printf("排列好的数组为；\n");
            printf("%d",nums[b]);
        }
    }
    return 0;
}



