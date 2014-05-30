#include "ComplementaryFilter.h"

ComplementaryFilter::ComplementaryFilter(float dt, float alpha, float beta)
{
    this->dt    = dt;
    this->alpha = alpha;
    this->beta  = beta;
}

ComplementaryFilter::~ComplementaryFilter(void)
{
}

void ComplementaryFilter::reset(void)
{
    this->angle = 0;
}

/*
 * All the values of acc_rate and gyro rate can be given either in radians or degrees
 * x_acc = (float)(x_acc_ADC – x_acc_offset) * x_acc_scale;
 * gyro  = (float)(gyro_ADC – gyro_offset)   * gyro_scale;
 */
float ComplementaryFilter::update(float acc_angle, float gyro_rate)
{
    this->angle = ( alpha * ( this->angle + (gyro_rate * dt) ) ) + (beta * acc_angle);
    return this->angle;
}