// ComplementaryFilter.cpp
#include "pch.h"
#include "ComplementaryFilter.h"

using namespace ComplementaryFilter;
//using namespace Platform;
//namespace required to support sensor and events
//using namespace Windows::Devices::Sensors;
//using namespace Windows::Foundation;
//using namespace Windows::System::Threading;

//using namespace Windows::Foundation::Collections;

//namespace for UI control
//using namespace Windows::UI::Xaml;
//using namespace Windows::UI::Xaml::Media;



void CompFilter::ComplementaryFilterInit(float dt, float alpha, float beta)
{
	this->dt = dt;
	this->alpha = alpha;
	this->beta = beta;
}



void CompFilter::reset(void)
{
	this->angle = 0;
}

/*
* All the values of acc_rate and gyro rate can be given either in radians or degrees
* x_acc = (float)(x_acc_ADC – x_acc_offset) * x_acc_scale;
* gyro  = (float)(gyro_ADC – gyro_offset)   * gyro_scale;
*/
float CompFilter::update(float acc_angle, float gyro_rate)
{
	this->angle = (alpha * (this->angle + (gyro_rate * dt))) + (beta * acc_angle);
	return this->angle;
}