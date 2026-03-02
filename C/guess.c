#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int secret, guess;
    
    // 初始化随机数
    srand(time(NULL));
    
    // 生成1-100的秘密数字
    secret = 1 + rand() % 100;
    
    printf("猜猜我想的数字（1-100）是多少？\n");
    
    do {
        printf("输入你的猜测：");
        scanf("%d", &guess);
        
        if (guess > secret) {
            printf("太大了！\n");
        } else if (guess < secret) {
            printf("太小了！\n");
        }
    } while (guess != secret);
    
    printf("恭喜你猜对了！\n");
    
    return 0;
}