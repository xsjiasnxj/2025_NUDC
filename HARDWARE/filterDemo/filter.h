/*
 * filter.h
 *
 *  Created on: 2023年5月4日
 *      Author: Mhr
 */

#ifndef INC_FILTER_H_
#define INC_FILTER_H_
typedef struct
{
    float vo0;
    float vo1;
    float vo2;//输出变量
    float vi0;
    float vi1;
    float vi2;//输入变量
    float ppf_Ts;
    float ppf_omega;
    float ppf_a0;
    float ppf_a2;
    float ppf_b0;
    float ppf_b1;
    float ppf_b2;
} pointpassfiltertype;

typedef struct
{
    unsigned char lead_lag;
    float phaser_omega;
    float phaser_rc;
    float phaser_angle;
    float phaser_Ts;
    float phaser_a0;
    float phaser_a1;
    float phaser_b0;
    float phaser_b1;
    float phaser_uo0;
    float phaser_uo1;
    float phaser_ui0;
    float phaser_ui1;

} phasertype;

typedef struct
{
    unsigned char high_low; //1=hpf,0=lpf
    float fof_omega;
    float fof_rc;
    float fof_Ts;
    float fof_ui0;
    float fof_ui1;
    float fof_uo0;
    float fof_uo1;
    float fof_a0;
    float fof_a1;
    float fof_b0;
    float fof_b1;
} firstordertype;

typedef struct
{
    unsigned char high_low; //1=hpf,0=lpf
    float vo0;
    float vo1;
    float vo2;
    float vi0;
    float vi1;
    float vi2;
    float sof_Ts;
    float sof_omega;
    float sof_a0;
    float sof_a1;
    float sof_a2;
    float sof_b0;
    float sof_b1;
    float sof_b2;
} secondordertype;

float mytan(float x);

void point_pass_filter_init(pointpassfiltertype *ppf, float omega, float Ts);

float point_pass_filter(pointpassfiltertype *ppf, float vi);

void phaser_init(phasertype *pt, float omega, float Ts, float angle);

float phaser(phasertype *pt, float ui);

void fof_init(firstordertype *fof, float omega, float Ts, unsigned char high_or_low);

float fof(firstordertype *fof, float ui);

void sof_init(secondordertype *sof, float omega, float Ts, unsigned char high_or_low);

float sof(secondordertype *sof, float vi);

#endif /* INC_FILTER_H_ */
