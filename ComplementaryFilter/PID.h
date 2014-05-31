class PID
{
public:
	PID(float kp, float ki, float kd, float dt);

	void setGains(float kp, float ki, float kd);
	void reset();
	float compute(float setPoint, float currentPoint);

	float getP();
	float getI();
	float getD();

private:
	float kp, ki, kd, _pid;
	float dt;
	float lastErro, lastSumErro;
	float integral;
};
