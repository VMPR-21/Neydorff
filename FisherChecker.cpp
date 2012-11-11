#include "FisherChecker.h"
#include "FisherCriteria.h"

FisherChecker::FisherChecker()
{
}

bool FisherChecker::Check(double min, int minF, double max, int maxF)
{
    double Ft = FisherCriteria::GetFisherVal(minF, maxF);
    double F = 0;

    if(min == 0)
        F = 0;
    else
        F =  max / min;

    return Ft > F;
}
