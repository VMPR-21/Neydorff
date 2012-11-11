#ifndef FISHERCHECKER_H
#define FISHERCHECKER_H
#include "IStaticCriteria.h"

class FisherChecker : public IStaticCriteria
{
public:
    FisherChecker();

    bool Check(double min, int freedom1, double max, int minF);
};

#endif // FISHERCHECKER_H
