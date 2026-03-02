#include <stdio.h>
int main(){
     // 数组
    int arr[10];//2,3,4,5,6,7,8,9,0,1//
    //len 元素个数//
    int len = sizeof(arr)/sizeof(arr[0]);
    for(int i = 0; i < len; i++){
        scanf("%d", arr + i);
    }
    int temp;
    //依次对下标对应元素进行寻找//
    for(int i = 0; i < len; i++) {
        printf("=== 第%d轮排序开始 ===\n\t", i+1);
        for(int j = 0;j < len - 1 - i; j++) {
            printf("比较 arr[%d]=%d 和 arr[%d]=%d: \n\t", j, arr[j], j+1, arr[j+1]);
            //如果前一个数大于后一个数
            if(arr[j] > arr[j + 1]){
                //将前一个数赋值给temp//
                temp = arr[j];
                //后一个数赋值给前一个数//
                arr[j] = arr[j + 1];
                //将temp赋值给后一个数//
                arr[j + 1] = temp;
            }
            }
        }
    

    printf("排序好的数组为 \n");
    for(int i = 0; i < len; i++){      
        printf("%d ",arr[i]);
    }    
    return 0;
}