#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>

int count_words(const char *text){
    int count = 0;
    int in_word = 0;

    //只在遇到空白字符并且刚刚结束一个单词时，才把单词计数加一，其余情况只更新是否处于这个单词的状态，不加计数
    //定义一个指针p指向当前单词，在指到停止符之前，依次读取单词中的字母
    for (const char *p = text; *p; ++p) {
        printf("[for] p = %p *p = '%c'  in_word = %d\n", p, *p, in_word);
        //用isspace函数检查当前指针是否指向空白
        if (isspace((unsigned char)*p)) {
            printf("[if-1]是空白字符\n");
            //如果在单词内部，计数器加1，指示器提示显示为0
            if (in_word) {
                count++;
                in_word = 0;
            //如果不在单词内部，则指示器提示为1
            }
        } else {
            in_word = 1;
        }
    }
    //在文章末尾没有空白，循环最后一次遇到的都是普通字母，于是in_word = 1,
    //for循环结束没有机会再遇到空白去触发计数加一，因此手动计数加一确保最后一个单词计数加一
    if (in_word) count++; 
    return count;
}

int main(void){
    int fd = open("example.txt", O_RDONLY);
    if(fd == -1){
        perror("open");
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1){
        perror("fstat");
        close(fd);
        return 1;
    }
    size_t size = st.st_size;

    char *buf = malloc(size + 1);

    if(!buf){
        perror("malloc");
        close(fd);
        return 1;
    }
    ssize_t rsize = read(fd, buf, size);
     if (rsize != (ssize_t)size) {
        perror("read");
        free(buf);
        close(fd);
        return 1;
    }
    buf[size] = '\0';               // 确保字符串终止

    close(fd);                      // 文件描述符用完即关

    /* 4. 统计并输出结果 */
    printf("单词总数：%d\n", count_words(buf));

    free(buf);
    return 0;
}