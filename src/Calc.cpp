#include <iostream>
#include "Calc.h"

int TimeCorrection(double offset, double time, double speed){
    double i, sum = 0;
    if (time <= 21276){
        for (i = 0; i < 11; ++i){
            sum = sum + offset * time/pow(speed,i);
            printf("%.5f\n",sum);
        } 
    }
    else{
        time = 43200 - time;
        for (i = 0; i < 11; ++i){
            sum = sum + offset * pow(-1,i) * time/pow(speed,i);
            printf("%.5f\n",sum);
        }   
    }
    return int(lround(sum));
    printf("%.5f",sum);
}
