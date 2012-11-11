#ifndef CENTRALORTOGONALREGRESSIONCOEFFICIENTTABLE_H
#define CENTRALORTOGONALREGRESSIONCOEFFICIENTTABLE_H

#include "RegressionCoefficientTable.h"

class CentralOrtogonalRegressionCoefficientTable : public RegressionCoefficientTable
{
public:
    static CentralOrtogonalRegressionCoefficientTable* Create(const int factorCount, const int interactionLevel);

protected:
    virtual void generateCoeffs();
    virtual void calcNormCoefs(const IFactorTable &xTable, const IResponseTable &yTable);
    virtual void calcNaturalCoefs(const IFactorTable &xTable, const IResponseTable &yTable);

private:
    CentralOrtogonalRegressionCoefficientTable(const int factorCount, const int interactionLevel);
};

#endif // CENTRALORTOGONALREGRESSIONCOEFFICIENTTABLE_H
