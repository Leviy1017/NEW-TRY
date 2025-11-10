gcc -fsanitize=address -g -O1 -Wall -Werror "$1" -o "${1%.*}"
#检查内存越界 显示错误行号 优化编译器级数0到3 3优化太多一般不用 显示潜在的编译告警