/*
 * filter.c
 *
 *  Created on: 2023年5月4日
 *      Author: Mhr
 */

#define pi 3.1415936535f

#define sqrt2 1.4142135623730951f

#include "stdio.h"
#include "stdlib.h"
#include "filter.h"
#include "arm_math.h"

float mytan(float x)
{
    float sinx = arm_sin_f32(x);
    float cosx = arm_cos_f32(x);
    if (cosx < 0.001f && cosx >= 0)
    {
        cosx = 0.001f;
    }
    if (cosx < -0.001f && cosx < 0)
    {
        cosx = -0.001f;
    }
    return sinx / cosx;

}

void point_pass_filter_init(pointpassfiltertype *ppf, float omega, float Ts)
{

    ppf->ppf_omega = omega;
    ppf->ppf_Ts = Ts;
    ppf->ppf_a0 = -4 * ppf->ppf_omega * ppf->ppf_Ts;
    ppf->ppf_a2 = 4 * ppf->ppf_omega * ppf->ppf_Ts;
    ppf->ppf_b0 =
            (ppf->ppf_omega) * (ppf->ppf_omega) * (ppf->ppf_Ts) * (ppf->ppf_Ts) - 4 * ppf->ppf_omega * ppf->ppf_Ts + 4;
    ppf->ppf_b1 = 2 * (ppf->ppf_omega) * (ppf->ppf_omega) * (ppf->ppf_Ts) * (ppf->ppf_Ts) - 8;
    ppf->ppf_b2 =
            (ppf->ppf_omega) * (ppf->ppf_omega) * (ppf->ppf_Ts) * (ppf->ppf_Ts) + 4 * (ppf->ppf_omega) * (ppf->ppf_Ts) +
            4;
    ppf->vi2 = 1;
    ppf->vi1 = 1;
    ppf->vi0 = 1;
    ppf->vo0 = 1;
    ppf->vo1 = 1;
    ppf->vo2 = 1;
}

float point_pass_filter(pointpassfiltertype *ppf, float vi)
{
    ppf->vi2 = ppf->vi1;
    ppf->vi1 = ppf->vi0;
    ppf->vi0 = vi;
    ppf->vo2 = ppf->vo1;
    ppf->vo1 = ppf->vo0;
    ppf->vo0 = (ppf->ppf_a0 * ppf->vi2 + ppf->ppf_a2 * ppf->vi0 - ppf->ppf_b0 * ppf->vo2 - ppf->ppf_b1 * ppf->vo1) /
               ppf->ppf_b2;
    return ppf->vo0;
}

void phaser_init(phasertype *pt, float omega, float Ts, float angle)
{
    pt->phaser_Ts = Ts;
    pt->phaser_angle = angle;
    pt->phaser_omega = omega;
    //angle为移相角度(弧度制)，大于0为输出超前于输入，小于等于0为输出滞后于输入。
    //angle范围：-pi<angle<pi。
    if (angle <= -1 * pi)
    {
        angle = -0.99f * pi;
    }
    if (angle >= pi)
    {
        angle = 0.99f * pi;
    }
    if (angle > 0)
    {
        pt->lead_lag = 1;
        pt->phaser_rc = mytan((pi - pt->phaser_angle) / 2.0f) / pt->phaser_omega;
        pt->phaser_a0 = -1 * (2 * pt->phaser_rc + pt->phaser_Ts);
        pt->phaser_a1 = 2 * pt->phaser_rc - pt->phaser_Ts;
        pt->phaser_b0 = pt->phaser_Ts - 2 * pt->phaser_rc;
        pt->phaser_b1 = 2 * pt->phaser_rc + pt->phaser_Ts;
    }
    else
    {
        pt->lead_lag = 0;
        pt->phaser_rc = mytan((-1 * pt->phaser_angle) / 2.0f) / pt->phaser_omega;
        pt->phaser_a0 = 2 * pt->phaser_rc + pt->phaser_Ts;
        pt->phaser_a1 = pt->phaser_Ts - 2 * pt->phaser_rc;
        pt->phaser_b0 = pt->phaser_Ts - 2 * pt->phaser_rc;
        pt->phaser_b1 = 2 * pt->phaser_rc + pt->phaser_Ts;
    }
    pt->phaser_ui0 = 0;
    pt->phaser_ui1 = 0;
    pt->phaser_uo0 = 0;
    pt->phaser_uo1 = 0;
}

float phaser(phasertype *pt, float ui)
{
    pt->phaser_ui1 = pt->phaser_ui0;
    pt->phaser_ui0 = ui;
    pt->phaser_uo1 = pt->phaser_uo0;
    pt->phaser_uo0 = ((pt->phaser_a1) * (pt->phaser_ui0) + (pt->phaser_a0) * (pt->phaser_ui1)
                      - (pt->phaser_b0) * (pt->phaser_uo1)) / pt->phaser_b1;
    return pt->phaser_uo0;

}

void fof_init(firstordertype *fof, float omega, float Ts, unsigned char high_or_low)
{
    fof->high_low = high_or_low;
    fof->fof_Ts = Ts;
    fof->fof_omega = omega;
    fof->fof_rc = 1.0f / (fof->fof_omega);
    if (fof->high_low == 0)
    {
        fof->fof_a0 = fof->fof_Ts;
        fof->fof_a1 = fof->fof_Ts;
        fof->fof_b1 = fof->fof_Ts + 2.0f * fof->fof_rc;
        fof->fof_b0 = fof->fof_Ts - 2.0f * fof->fof_rc;
    }
    else
    {
        fof->fof_a0 = -2.0f * fof->fof_rc;
        fof->fof_a1 = 2.0f * fof->fof_rc;
        fof->fof_b1 = fof->fof_Ts + 2.0f * fof->fof_rc;
        fof->fof_b0 = fof->fof_Ts - 2.0f * fof->fof_rc;
    }
    fof->fof_ui0 = 0;
    fof->fof_ui1 = 0;
    fof->fof_uo0 = 0;
    fof->fof_uo1 = 0;

}

float fof(firstordertype *fof, float ui)
{
    fof->fof_ui1 = fof->fof_ui0;
    fof->fof_ui0 = ui;
    fof->fof_uo1 = fof->fof_uo0;
    fof->fof_uo0 = (fof->fof_a1 * fof->fof_ui0 + fof->fof_a0 * fof->fof_ui1 - fof->fof_b0 * fof->fof_uo1) / fof->fof_b1;
    return fof->fof_uo0;
}

void sof_init(secondordertype *sof, float omega, float Ts, unsigned char high_or_low)
{
    sof->high_low = high_or_low;
    sof->sof_omega = omega;
    sof->sof_Ts = Ts;
    sof->sof_b0 = 4 - sqrt2 * (sof->sof_Ts) * (sof->sof_omega) +
                  (sof->sof_omega) * (sof->sof_omega) * (sof->sof_Ts) * (sof->sof_Ts);
    sof->sof_b1 = 2 * (sof->sof_omega) * (sof->sof_omega) * (sof->sof_Ts) * (sof->sof_Ts) - 8;
    sof->sof_b2 = (sof->sof_omega) * (sof->sof_omega) * (sof->sof_Ts) * (sof->sof_Ts) +
                  sqrt2 * (sof->sof_Ts) * (sof->sof_omega) + 4;
    if (sof->high_low == 0)
    {
        sof->sof_a0 = (sof->sof_omega) * (sof->sof_omega) * (sof->sof_Ts) * (sof->sof_Ts);
        sof->sof_a1 = 2 * sof->sof_a0;
        sof->sof_a2 = sof->sof_a0;
    }
    else
    {
        sof->sof_a0 = 4.0f;
        sof->sof_a1 = -8.0f;
        sof->sof_a2 = 4.0f;
    }
    sof->vi2 = 1.0f;
    sof->vi1 = 1.0f;
    sof->vi0 = 1.0f;
    sof->vo0 = 1.0f;
    sof->vo1 = 1.0f;
    sof->vo2 = 1.0f;
}

float sof(secondordertype *sof, float vi)
{
    sof->vi2 = sof->vi1;
    sof->vi1 = sof->vi0;
    sof->vi0 = vi;
    sof->vo2 = sof->vo1;
    sof->vo1 = sof->vo0;
    sof->vo0 = (sof->sof_a0 * sof->vi2 + sof->sof_a1 * sof->vi1 + sof->sof_a2 * sof->vi0 - sof->sof_b0 * sof->vo2 -
                sof->sof_b1 * sof->vo1) / sof->sof_b2;
    return sof->vo0;
}
