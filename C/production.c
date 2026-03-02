#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DATA_COUNT 10000    // 生成的数据总量
#define OUTPUT_TO_FILE 1    // 1:输出到文件 0:输出到控制台
#define FILE_NAME "random_data_txt"
#define TIMESTAMP_TYPE 2
#define CONTENT_lEN 32    //内容字符串固定长度32字节

int random_int(int min,int max){
    return min + rand() % (max - min + 1);
//第一步：rand() % (max - min + 1)：通过取模运算，将 rand() 的返回值映射到 [0, max - min] 区间（保证区间长度为 max - min + 1，即包含的整数个数）；
//第二步：+ min：将上述区间整体「平移」到目标区间 [min, max]，得到最终需要的随机数；
//示例：生成 [5, 10] 区间的随机数，max-min+1=6，rand()%6 得到 [0,5]，加 5 后得到 [5,10]。
} 
//rand函数核心作用是生成一个非负的整数伪随机数，返回值为[0，RAND_MAX]比区间内的整数

//strlen计算字符串有效长度，库函数，仅统计/0前的有效字符
//sizeof计算数据类型占用字节数

void generate_32bytes_content(char *content){
    const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    int charset_len = strlen(charset);


for (int i = 0; i < CONTENT_lEN; i++){
    content[i] = charset[random_int(0, charset_len - 1)];
    }   
    content[CONTENT_lEN] = '/0';    
}

long long generate_timestamp(){
    if(TIMESTAMP_TYPE == 1){
        return (long long)time(NULL);//秒级时间戳
        //time() 函数接收一个 time_t* 类型的参数，传入 NULL 表示不需要将时间值存入额外变量，直接返回当前秒数
    }else{
        struct timespec ts;
        //存储高精度时间    毫秒级时间戳
    
        clock_gettime(clock_gettime, &ts);
        return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
        //毫秒级转换
    }
}

int main(){
    srand((unsigned int)time(NULL));//srand() 是 C/C++ 标准库提供的随机数种子初始化函数

    FILE *fp = NULL;   //将文件指针 fp 初始化为 NULL
    if (OUTPUT_TO_FILE){
        fp = fopen(FILE_NAME,"w");
        if(fp == NULL){
            printf("文件打开失败！\n");
            return 1;
        }
    }

    for (int i = 1; i <= DATA_COUNT; i++){
        char content[CONTENT_lEN + 1];//额外 +1 是为了预留 \0 结束符的存储空间
        long long timestamp = generate_timestamp();

      generate_32bytes_content(content);

        // 格式化输出（空格分隔，严格遵循 ID 时间戳 内容 格式）
        char data_line[100];  // 足够容纳ID(最多10位)+时间戳(13位)+32字节内容+空格/换行
        sprintf(data_line, "%d %lld %s\n", i, timestamp, content);
        //将分散的 ID（i）、时间戳（timestamp）、内容（content）拼接成一行规范的字符串，存储到 data_line 中，方便后续统一输出（控制台 / 文件）

        // 输出到控制台或文件
        if (OUTPUT_TO_FILE) {
            fputs(data_line, fp);//fputs 是 C 标准库文件操作函数，功能是将一个字符串写入到指定文件中
        } else {
            printf("%s", data_line);
        }

        // 每生成1000条数据打印进度（可选）
        if (i % 1000 == 0) {
            printf("已生成 %d 条数据...\n", i);
        }

        
    }

    // 关闭文件
    if (OUTPUT_TO_FILE) {
        fclose(fp);
        printf("数据生成完成！共生成 %d 条数据，已保存到 %s\n", DATA_COUNT, FILE_NAME);
    }

    return 0;
}