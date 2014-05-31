#include "stddef.h"

const float dt = 0.005;

Gyroscope gyroscope(new L3G4200D(p5, p6, p7, p8), 0.0175, dt);;
Accelerometer accelerometer(new ADXL345(p5, p6, p7, p11), 256);;
ComplementaryFilter ComplementaryFilterX(dt, 0.9995, 0.0005);;
ComplementaryFilter ComplementaryFilterY(dt, 0.9995, 0.0005);;
PID pidX(0.0007f, 0.008f, 0.0004f, dt);
PID pidY(0.0007f, 0.008f, 0.0004f, dt);

bool compute_pid = false;
bool motors_armed = false;
bool start_update_loop = false;

float accX = 0.0;
float accY = 0.0;
float gyroX = 0.0;
float gyroY = 0.0;
float setPoint_x = 0.0;
float setPoint_y = 0.0;
float control_x = 0.0;
float control_y = 0.0;

float ComplementarAngle_x = 0.0;
float ComplementarAngle_y = 0.0;

float heading = 0.0;
float altura = 0.0;

int state = 0;
//float motorCoefA[5] = {1.4071, -2.7620, 1.9864, 0.3664, 0.0150};
//float motorCoefB[5] = {1.4433, -2.8580, 2.0790, 0.3207, 0.0111};
//float motorCoefC[5] = {1.4736, -2.8773, 2.1176, 0.2639, 0.0131};
//float motorCoefD[5] = {1.5427, -2.9893, 2.1539, 0.2750, 0.0119};

//Motor motorA(p26, 42, motorCoefA, 15, 60);
//Motor motorC(p24, 42, motorCoefC, 15, 60);
//Motor motorB(p25, 42, motorCoefB, 15, 60);
//Motor motorD(p23, 42, motorCoefD, 15, 60);
Motor motorA(p26, 42);
Motor motorC(p24, 42);
Motor motorB(p25, 42);
Motor motorD(p23, 42);

//Dummy
DigitalOut dummy(p21);

DigitalOut powerLed(LED1);
DigitalOut initLed(LED2);
DigitalOut statusLed1(LED3);
DigitalOut statusLed2(LED4);

Ticker updater;

volatile unsigned int sysCount;
volatile unsigned int pwmCount;
volatile unsigned int mainCount;
volatile unsigned int loop_time;

//SerialBuffered pc(USBTX, USBRX);
SerialBuffered pc(p28, p27);
char buffer[64];
char cmd = CMD_WAITING;
/*
* Sets the Motors
*/
void motors_setPower()
{

	motorB.setPowerLin(FLIGHT_THRESHOLD - (control_x * 100) + heading + altura);
	motorD.setPowerLin(FLIGHT_THRESHOLD + (control_x * 100) + heading + altura);

	motorA.setPowerLin(FLIGHT_THRESHOLD - (control_y * 100) - heading + altura);
	motorC.setPowerLin(FLIGHT_THRESHOLD + (control_y * 100) - heading + altura);

}

/*
* Update Loop
* Periodically Occurs every 5 milliseconds
*/
void update_lood()
{
	gyroscope.update();
	accelerometer.update();

	accX = accelerometer.getDegreesAngleX();
	accY = accelerometer.getDegreesAngleY();
	gyroX = (-1) * gyroscope.getDegreesX();
	gyroY = gyroscope.getDegreesY();

	//gyroAngle = gyroscope->getAngleY();

	ComplementarAngle_x = ComplementaryFilterX.update(accY, gyroX);
	ComplementarAngle_y = ComplementaryFilterY.update(accX, gyroY);


	if (compute_pid)
	{
		control_x = pidX.compute(setPoint_x, ComplementarAngle_x);
		control_y = pidY.compute(setPoint_y, ComplementarAngle_y);
	}

	motors_setPower();
}

/*
* PWM Loop
* Periodically Occurs every 1 millisecond
*/
void pwm_lood()
{
	if (pwmCount == 2 && !motors_armed)
	{
		dummy = 0;
	}
	else if (pwmCount == 4 && motors_armed)
	{
		dummy = 0;
	}
	else if (pwmCount == 40)//tempo igual a '20' e '0' (termina os 20 mili e inicia os próximos 20 mili) 
	{
		dummy = 1;
		pwmCount = 0;
	}
}

/*
* Main Loop
* Handle the update loop and the pwm loop
*/
void update()
{
	sysCount += 1;
	pwmCount += 1;
	if (sysCount >= 10 && start_update_loop)
	{
		update_lood();
		sysCount = 0;
	}

	pwm_lood();
}

bool setPointAdjustment(int currentLoopTime)
{
	bool reset = false;
	/* 2 graus por segundo
	2 - 1
	taxa - 0.010 -> loop_time     */

	float taxa = 0.04;

	switch (state) /* Máquina de estados */
	{
	case 0:
	case 6:
	case 7:
	case 8:
	case 14:
	case 15:
		if (setPoint_x < 0)
			setPoint_x += taxa;
		else if (setPoint_x > 0)
			setPoint_x -= taxa;

		if (setPoint_y < 0)
			setPoint_y += taxa;
		else if (setPoint_y > 0)
			setPoint_y -= taxa;
		break;
	case 1:
	case 2:
		if (setPoint_x < 20)
			setPoint_x += taxa;
		else if (setPoint_x > 20)
			setPoint_x -= taxa;
		break;
	case 3:
	case 4:
	case 5:
		if (setPoint_x < -20)
			setPoint_x += taxa;
		else if (setPoint_x > -20)
			setPoint_x -= taxa;
		break;
	case 9:
	case 10:
		if (setPoint_y < 20)
			setPoint_y += taxa;
		else if (setPoint_y > 20)
			setPoint_y -= taxa;
		break;
	case 11:
	case 12:
	case 13:
		if (setPoint_y < -20)
			setPoint_y += taxa;
		else if (setPoint_y > -20)
			setPoint_y -= taxa;
		break;

	}


	if (currentLoopTime >= 5000)
	{
		state++;
		if (state > 15)
			state = 0;
		reset = true;
	}
	return reset;
}

/*
* Main
*/
int main()
{
	pc.baud(PC_BAUD_RATE);

	//Leds de controle
	powerLed = 0;
	initLed = 0;
	statusLed1 = 0;
	statusLed2 = 0;

	//Contadores
	sysCount = 0;
	pwmCount = 0;
	mainCount = 0;

	//Variáveis lógicas
	start_update_loop = false;
	motors_armed = false;
	/*
	gyroscope = new Gyroscope(new L3G4200D(p5, p6, p7, p8), 0.0175, dt);
	accelerometer = new Accelerometer(new ADXL345(p5, p6, p7, p11), 256);
	ComplementaryFilterX = new ComplementaryFilter(dt, 0.9995, 0.0005);
	ComplementaryFilterY = new ComplementaryFilter(dt, 0.9995, 0.0005);
	*/
	powerLed = 1;

	wait(10);
	accelerometer.updateZeroRates();
	gyroscope.updateZeroRates();

	initLed = 1;

	motors_setPower();
	updater.attach(&update, 0.0005);
	wait(2);
	motors_armed = true;
	wait(7);

	//msg_update = true;
	compute_pid = true;
	start_update_loop = true;
	loop_time = 10;
	int i = 0;
	int j = 0;
	while (true)
	{
		wait_ms(loop_time);
		j += loop_time;
		if (setPointAdjustment(j))
			j = 0;

		/*
		if (pc.readable())
		cmd = pc.getc();

		switch (cmd)
		{
		case CMD_UP:
		heading+= 0.5;
		sprintf((char*)buffer, "heading %f\r\n", heading);
		pc.writeText(buffer);
		break;

		case CMD_DOWN:
		heading-= 0.5;
		sprintf((char*)buffer, "heading %f\r\n", heading);
		pc.writeText(buffer);
		break;

		case CMD_UP2:
		altura += 0.5;
		sprintf((char*)buffer, "altura %f\r\n", altura);
		pc.writeText(buffer);
		break;

		case CMD_DOWN2:
		altura -= 0.5;
		sprintf((char*)buffer, "altura %f\r\n", altura);
		pc.writeText(buffer);
		break;

		}

		cmd = CMD_WAITING;
		*/
		sprintf((char*)buffer, "%f %f %f %f \n", ComplementarAngle_x, setPoint_x, ComplementarAngle_y, setPoint_y);
		pc.writeText(buffer);

		i += loop_time;
		if (i >= 200)
		{
			statusLed2 = statusLed1;
			statusLed1 = !statusLed1;
			i = 0;
		}
	}
}
