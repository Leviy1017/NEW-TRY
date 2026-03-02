#include <stdio.h>

#define PI 3.14159

float area ( float r ) {
    return  PI * r * r;
} 

int main(){
    
    float r = 0.0;
    float s = 0.0;
    printf("输入半径\n");
    scanf("%f ", &r);

    s = area(r);

    printf("s: %f\n", s);

    return 0;
}

