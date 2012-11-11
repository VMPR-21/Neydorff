#include "CentralOrtogonalRegressionCoefficientTable.h"
#include "RegressionFunctions.h"

CentralOrtogonalRegressionCoefficientTable::CentralOrtogonalRegressionCoefficientTable(const int factorCount, const int interactionLevel)
    : RegressionCoefficientTable(factorCount, interactionLevel)
{
}

CentralOrtogonalRegressionCoefficientTable* CentralOrtogonalRegressionCoefficientTable::Create(const int factorCount, const int interactionLevel)
{
    CentralOrtogonalRegressionCoefficientTable *table = new CentralOrtogonalRegressionCoefficientTable(factorCount, interactionLevel);
    table->generateCoeffs();
    return table;
}

void CentralOrtogonalRegressionCoefficientTable::generateCoeffs()
{
    setCoeffs(generateCOCPCoeffs(factorCount(), interactionLevel()));
}

void CentralOrtogonalRegressionCoefficientTable::calcNormCoefs(const IFactorTable &xTable, const IResponseTable &yTable)
{
    calcCOCPNormCoefs(xTable, yTable, *this);
}

void CentralOrtogonalRegressionCoefficientTable::calcNaturalCoefs(const IFactorTable &xTable, const IResponseTable &yTable)
{
    calcCOCPNaturalCoefs(xTable, yTable, *this);
}
