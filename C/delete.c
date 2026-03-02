/*给你一个 非严格递增排列 的数组 nums ，请你 原地 删除重复出现的元素，使每个元素 只出现一次 ，返回删除后数组的新长度。
元素的 相对顺序 应该保持 一致 。然后返回 nums 中唯一元素的个数。

考虑 nums 的唯一元素的数量为 k ，你需要做以下事情确保你的题解可以被通过：

更改数组 nums ，使 nums 的前 k 个元素包含唯一元素，并按照它们最初在 nums 中出现的顺序排列。nums 的其余元素与 nums 的大小不重要。
返回 k 。
判题标准:
系统会用下面的代码来测试你的题解:
示例 1：

输入：nums = [1,1,2]
输出：2, nums = [1,2,_]
解释：函数应该返回新的长度 2 ，并且原数组 nums 的前两个元素被修改为 1, 2 。不需要考虑数组中超出新长度后面的元素。
示例 2：

输入：nums = [0,0,1,1,1,2,2,3,3,4]
输出：5, nums = [0,1,2,3,4]
解释：函数应该返回新的长度 5 ， 并且原数组 nums 的前五个元素被修改为 0, 1, 2, 3, 4 。不需要考虑数组中超出新长度后面的元素。*/

#include<stdio.h>
#include<stdlib.h>

// 我们的代码或者说工程代码，讲究低耦合和高内聚
// 低耦合：是把一个功能写到一个函数里，而不是在main函数里全部写完
// 或者说一个函数里，存在多个功能
// 这样做的好处是，如果你发现一个功能调用后不是你预期的产物
// 你就可以只更改这个功能函数，而不需要再找别的地方
// 内聚：各功能模块之间的联系程度

int removeNum(int* nums, int numsSize) {
    // 如果数组为空，直接返回0
    if (numsSize == 0) return 0;

    // 数组不为空
    // writeindex 始终指向下一个唯一元素应该存放的位置
    // 它一开始指向数组的第2个位置（下标1）
    // 因为第1个元素（下标0）一定是唯一的，不需要处理
    // 从第2个元素开始判断他是不是重复的
    // 如果不重复->把它放到writeindex指向的位置，然后writeindex往后移动一位
    // 如果重复->跳过，不动writeindex
    int writeIndex = 1; 

    // readindex 当前元素
    // writeindex 比较完后要写的元素
    for (int readIndex = 1; readIndex < numsSize; readIndex++) {
        // 当前元素与前一个唯一元素不同，说明是一个新的唯一元素
        if (nums[readIndex] != nums[writeIndex - 1]) {
            nums[writeIndex] = nums[readIndex];
            writeIndex++;
        }
    }
    return writeIndex; // writeindex即为唯一元素的个数
}

int main() {
    // TODO:数组可以动态拓展或缩小->参考malloc
    int nums[5];
    int numsSize = sizeof(nums) / sizeof(nums[0]);
    for (int i = 0; i < numsSize; i++) {
        scanf("%d", nums + i);
    }

    // 数组中不重复元素的个数
    int uniqueNum = removeNum(nums, numsSize);
    printf("唯一元素的个数%d\n", uniqueNum);

    printf("修改后的数组：[");
    for (int i = 0; i < uniqueNum; i++) {
        printf("%d", nums[i]);
        if (i < uniqueNum - 1) printf(",");
    }
    printf("]\n");
    return 0;
}













 

