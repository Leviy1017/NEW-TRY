#include <stdio.h>
#include <stdlib.h>

int main(){
    //定义一个指针数组，存储字符串常量
    const char *s[] = {"man", "woman", "girl", "boy", "sister"};
    //定义一个指向字符指针的指针变量
    const char **q;
    int k;

    //遍历，并通过指向指针的指针输出每个字符串
    for (k = 0; k < 5; k++){
        q = &s[k];//让指针q指向字符指针数组中第k个元素的地址
        printf("%s\n", *q);//解引用q，输出对应的字符串
    }
    return 0;
}