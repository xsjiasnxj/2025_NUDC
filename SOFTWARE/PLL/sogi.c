//
// Created by MHR on 2023-7-8.
//

#include "sogi.h"

void sogi_para_update(sogitype *st, float omega)
{
    st->sogi_omega = omega;
    st->sogi_x = 2.0f * st->sogi_k * st->sogi_omega * st->sogi_Ts;
    st->sogi_y = st->sogi_omega * st->sogi_Ts * st->sogi_omega * st->sogi_Ts;
    st->sogi_temp = 1.0f / (st->sogi_x + st->sogi_y + 4.0f);
    st->sogi_b0 = st->sogi_x * st->sogi_temp;
    st->sogi_b2 = (-1.0f) * st->sogi_b0;
    st->sogi_a1 = (2.0f) * (4.0f - st->sogi_y) * st->sogi_temp;
    st->sogi_a2 = (st->sogi_x - st->sogi_y - 4.0f) * st->sogi_temp;
    st->sogi_qb0 = (st->sogi_k * st->sogi_y) * st->sogi_temp;
    st->sogi_qb1 = st->sogi_qb0 * (2.0f);
    st->sogi_qb2 = st->sogi_qb0;
}

void sogi_init(sogitype *st, float Ts, float omega)
{
    st->sogi_Ts = Ts;
    st->sogi_k = 0.5f;
    st->sogi_omega = omega;
    st->sogi_x = 2.0f * st->sogi_k * st->sogi_omega * st->sogi_Ts;
    st->sogi_y = st->sogi_omega * st->sogi_Ts * st->sogi_omega * st->sogi_Ts;
    st->sogi_temp = 1.0f / (st->sogi_x + st->sogi_y + 4.0f);
    st->sogi_b0 = st->sogi_x * st->sogi_temp;
    st->sogi_b2 = (-1.0f) * st->sogi_b0;
    st->sogi_a1 = (2.0f) * (4.0f - st->sogi_y) * st->sogi_temp;
    st->sogi_a2 = (st->sogi_x - st->sogi_y - 4) * st->sogi_temp;
    st->sogi_qb0 = (st->sogi_k * st->sogi_y) * st->sogi_temp;
    st->sogi_qb1 = st->sogi_qb0 * (2.0f);
    st->sogi_qb2 = st->sogi_qb0;
}

void sogi(sogitype *st, float vi, float *vo, float *qvo)
{
    //更新序列值
    st->vi2 = st->vi1;
    st->vi1 = st->vi0;
    st->vi0 = vi;
    st->vo2 = st->vo1;
    st->vo1 = st->vo0;
    //对应离散序列
    st->vo0 = st->sogi_b0 * st->vi0 + st->sogi_b2 * st->vi2 + (st->sogi_a1 * st->vo1) + (st->sogi_a2 * st->vo2);
    st->qvo2 = st->qvo1;
    st->qvo1 = st->qvo0;
    //对应离散序列
    st->qvo0 =
            (st->sogi_qb0 * st->vi0) + (st->sogi_qb1 * st->vi1) + (st->sogi_qb2 * st->vi2) + (st->sogi_a1 * st->qvo1) +
            (st->sogi_a2 * st->qvo2);
    //输出
    *vo = st->vo0;
    *qvo = st->qvo0;
}
