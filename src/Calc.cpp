#include <iostream>
#include "Calc.h"

int TimeCorrection(double offset, double time, double speed){               // offset = steps/1sec, time in secs, speed in clock secs/real sec
    int i;
    double sum = 0;
    if (time <= 21276){
        for (i = 0; i < 11; ++i){
            sum += offset * time/pow(speed,i);
            // printf("%.5f\n",sum);
        } 
    }
    else{
        time = 43200 - time;
        for (i = 0; i < 11; ++i){
            sum += offset * pow(-1,i) * time/pow(speed,i);
            // printf("%.5f\n",sum);
        }   
    }
    return int(lround(sum));
    printf("%.5f",sum);
}
