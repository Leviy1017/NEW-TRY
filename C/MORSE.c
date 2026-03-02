#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1000

// 摩尔斯码表（a-z）
const char* morse_table[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
    "..-", "...-", ".--", "-..-", "-.--", "--.."
};

// 二叉树节点
typedef struct Node {
    char letter;
    struct Node* dot;
    struct Node* dash;
} Node;

// 创建新节点
Node* create_node(char letter) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->letter = letter;
    node->dot = node->dash = NULL;
    return node;
}

// 插入摩尔斯码到树中，首先定义root为树本身，code为字符，letter为对应字母
//先将root数据给到指针curr
//变量i进行遍历直到结束
//如果i遇到了.的信号则向左走，假如左边为空则进行建立分叉，然后移动指针到子节点dot
//同理，右边也是这样操作，移动指针到字节的dash
//最终字母会和字符顺序一一对应
void insert_morse(Node* root, const char* code, char letter) {
    Node* curr = root;
    for (int i = 0; code[i] != '\0'; i++) {
        if (code[i] == '.') {
            if (!curr->dot) {curr->dot = create_node(0);
            curr = curr->dot;}
        } else if (code[i] == '-') {
            if (!curr->dash) {curr->dash = create_node(0);
            curr = curr->dash;}
        }
    }
    curr->letter = letter;
}

// 插入摩尔斯码到树中
//初始化当前指针，遍历摩尔斯字符串，如果遇到点信号，向左建立一个dot仓库并进入，划信号同理。最终指针按路径走到最终位置。
//（在数中开辟路径并在终点放置对应字母）
void insert_morse(Node* root, const char* code, char letter) {
    Node* curr = root;
    for (int i = 0; code[i] != '\0'; i++) {
        if (code[i] == '.') {
            if (!curr->dot) curr->dot = create_node(0);
            curr = curr->dot;
        } else if (code[i] == '-') {
            if (!curr->dash) curr->dash = create_node(0);
            curr = curr->dash;
        }
    }
    curr->letter = letter;
}

// 构建摩尔斯树
//建立root为所有路径起点
//遍历26个字母
//查看第i个字母的路径，‘a’+i确定字母，调用insert_morse告诉指针路径
//最后返回起点
//相当于为每一个字母创建对应的路径，按照指示便可以找到对应字母
Node* build_morse_tree() {
    Node* root = create_node(0);
    for (int i = 0; i < 26; i++) {
        insert_morse(root, morse_table[i], 'a' + i);
    }
    return root;
}

// 解码单个摩尔斯码字符串（如 ".-...-...." → "ace"）
//使用之前建立的树来查找对应字母
//out_idx相当于一个空箱子
//首先指针观看每一个字符对应的路径直到看完为止，当出现空格时表明当前路径结束，然后将找到的字母放进空箱子out_idx，并continue继续查看下一个字符
//之后按照方向走对应的路径，如果发现路径走不通，即curr=NULL，则返回。 如果指针仍有找到的字母，则将其放入空箱子并用\0让空箱子密封起来，表示结束放置
void decode_morse(Node* root, const char* morse, char* output) {
    Node* curr = root;
    int out_idx = 0;

    for (int i = 0; morse[i] != '\0'; i++) {
        if (morse[i] == ' ') {
            if (curr != root && curr->letter != 0) {
                output[out_idx++] = curr->letter;
                curr = root;
            }
            continue;
        }

        if (morse[i] == '.') curr = curr->dot;
        else if (morse[i] == '-') curr = curr->dash;

        if (curr == NULL) {
            printf("非法摩尔斯码序列\n");
            return;
        }
    }

    if (curr != root && curr->letter != 0) {
        output[out_idx++] = curr->letter;
    }

    output[out_idx] = '\0';
}

// 编码单词为摩尔斯码（用空格分隔字母，用 / 分隔单词）
//word为要翻译的单词 output为翻译好的结果 table为字典
//首先将output初始化 一个一个查看要翻译的单词直至单词结束
//检查当前字母是不是小写字母a到z，使用morse_table找到对应摩尔斯码，计算字母在table中的位置
//使用strcat将查找的莫尔斯码写到结果output并加一个空格分隔不同的字母
//测量结果纸的长度，检查最后一个字符是不是空格，如果是，就擦掉他
void encode_word(const char* word, char* morse_output) {
    morse_output[0] = '\0';
    for (int i = 0; word[i] != '\0'; i++) {
        if (word[i] >= 'a' && word[i] <= 'z') {
            strcat(morse_output, morse_table[word[i] - 'a']);
            strcat(morse_output, " ");
        }
    }
    // 去掉最后一个空格
    int len = strlen(morse_output);
    if (len > 0 && morse_output[len - 1] == ' ') {
        morse_output[len - 1] = '\0';
    }
}

// 测试
int main() {
    Node* root = build_morse_tree();

    const char* test_word = "cab";
    char morse_encoded[MAX_LEN];
    encode_word(test_word, morse_encoded);
    printf("编码 %s → %s\n", test_word, morse_encoded);

    char decoded[MAX_LEN];
    decode_morse(root, morse_encoded, decoded);
    printf("解码 %s → %s\n", morse_encoded, decoded);

    // 测试多个单词拼接（用 / 分隔）
    const char* multi = ".- -... / -.-.";  // "a b / c"
    char decoded_multi[MAX_LEN];
    decode_morse(root, multi, decoded_multi);
    printf("解码多词 %s → %s\n", multi, decoded_multi);

    return 0;
}


