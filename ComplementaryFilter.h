#ifndef COMPFILTER_H
#define COMPFILTER_H

/*
#include <stdlib.h>
#include <math.h>
*/
class ComplementaryFilter
{
    public:
    
        ComplementaryFilter(float dt, float alpha, float beta);
        ~ComplementaryFilter(void);
        float update(float acc_rate, float gyro_rate);
            
    private:
    
        void reset(void);
        float dt, alpha, beta;
        float angle;
};

#endif
