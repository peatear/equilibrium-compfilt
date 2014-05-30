#pragma once

namespace ComplementaryFilter
{
	public ref class CompFilter sealed
    {
    public:

		void ComplementaryFilterInit(float dt, float alpha, float beta);
		float update(float acc_rate, float gyro_rate);
		void reset(void);

	private:

		float dt, alpha, beta;
		float angle;
    };
}