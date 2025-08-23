#include<stdio.h>

// 函数声明
int bubble_sort(int arr[10], int len);

int binarySearch(int arr[10], int left, int right, int target) {
    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] == target)
            return mid;

        if (arr[mid] < target)
            left = mid + 1;
        else
            right = mid - 1;
    }
    return -1;
}

int bubble_sort(int arr[10], int len) {
    int temp;
    // 外层循环控制轮数
    for (int i = 0; i < len - 1; i++) {
        printf("=== 第%d轮排序开始 ===\n", i + 1);
        // 内层循环进行比较和交换
        for (int j = 0; j < len - 1 - i; j++) {
            printf("比较 arr[%d]=%d 和 arr[%d]=%d: ", j, arr[j], j + 1, arr[j + 1]);
            // 如果前一个数大于后一个数
            if (arr[j] > arr[j + 1]) {
                // 交换元素
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                printf("需要交换\n");
            } else {
                printf("不需要交换\n");
            }
        }
    }
    
    printf("排序好的数组为: ");
    for (int i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    return 0;
}

int main() {
    int arr[10];
    int len = 10;
    int target, result;

    printf("请输入十个整数：");
    for (int i = 0; i < len; i++) {
        scanf("%d", &arr[i]);
    }
    
    // 调用冒泡排序函数
    bubble_sort(arr, len);

    printf("排序后的数组: ");
    for (int i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    printf("请输入要查找的数字：");
    scanf("%d", &target);

    result = binarySearch(arr, 0, len - 1, target);
    if (result != -1) {
        printf("找到数字 %d, 位置是第 %d 个\n", target, result + 1);
    } else {
        printf("未找到数字 %d\n", target);
    }

    return 0;
}