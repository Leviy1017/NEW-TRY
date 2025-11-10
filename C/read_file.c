#include<stdio.h>
#include<stdlib.h>

int main(){
    FILE *fp;
    fp = fopen("example.txt","w");
    if (fp == NULL){
        perror("文件创建失败");
        return 1;
    }


    fprintf(fp, "Hello, fopen!\n");

    fclose(fp);
    printf("文件操作成功\n");
    return 0;
}


