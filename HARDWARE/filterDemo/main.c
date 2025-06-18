#include "filter.h"
#include "stdio.h"
#include "math.h"

firstordertype fof1;
secondordertype sof1;
#define pi 3.1415926535f
#define sample_time 0.001f    //采样周期1KHz
#define dc 5.0f        //模拟直流5V输入

int main()
{
    float sine = 0.0f;
    float t = 0.0f;
    float out1 = 0, out2 = 0;
    fof_init(&fof1, 10 * 2 * pi, sample_time, 0);    //设置fof1为截止频率10Hz的一阶低通
    sof_init(&sof1, 10 * 2 * pi, sample_time, 0);    //设置sof1为截止频率10Hz的二阶低通
    while (getchar() != 'q')
    {
        t = t + sample_time;
        sine = sinf(100 * pi * t);    //模拟50Hz的干扰信号
        out1 = fof(&fof1, dc + sine);
        out2 = sof(&sof1, dc + sine);
        printf("1st Order=%f, 2nd Order=%f\n", out1, out2);
    }
    return 0;
}