#include "PID.h"

PID::PID(float kp, float ki, float kd, float dt)
{
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;
	this->dt = dt;

	reset();
}

void PID::reset()
{
	lastErro = 0.0f;
	lastSumErro = 0.0f;
	_pid = 0.0f;

	integral = 0.0f;

}

void PID::setGains(float kp, float ki, float kd)
{
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;
}

float PID::compute(float setPoint, float currentPoint)
{
	float error = setPoint - currentPoint;
	float derivative = (error - lastErro) / dt;
	integral = (integral + (error*dt));

	float contribuicao = (kp*error) + (ki*integral) + (kd*derivative);



	//_pid = contribuicao;

	lastErro = error;
	//lastSumErro += error;

	return contribuicao;
}

float PID::getP()
{
	return kp;
}

float PID::getI()
{
	return ki;
}

float PID::getD()
{
	return kd;
}