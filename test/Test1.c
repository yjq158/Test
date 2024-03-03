//
// Created by yaojq on 2024/1/18.
//
#include <stdio.h>
#include "../include/Runtime.h"
int main() {
    int x;
    DSE_Input(x);
    int c = 10;
    int y = 1;
    for(int i=0; i<x; i++){
        y *= 2;
    }
    if (y % 8 == 0) {
        x = x / (c-c);
    }
    return 0;
}
