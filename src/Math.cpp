#include <iostream>
#include "Math.h"

int TimeCorrection(double offset, double time, double speed){
    double i, sum = 0;
    for (i = 0; i < 11; ++i){
        sum = sum + offset * time/pow(speed,i);
        printf("%.5f\n",sum);
    }
    return int(lround(sum));
    printf("%.5f",sum);
}

// int main()
// {
//     int a = 3600, b = 10, c = 10;
//     return series(c,a,b);
// }
