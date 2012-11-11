#include "GradientRegressionCoefficientTable.h"
#include <assert.h>
#include <vector>
#include <limits>
#include <cmath>
#include "bCoeff.h"
#include "IResponseTable.h"
#include "RegressionFunctions.h"
#include "RegressionCoefficientTable.h"

using namespace std;

GradientRegressionCoefficientTable::GradientRegressionCoefficientTable(const int factorCount, const int interactionLevel)
    : RegressionCoefficientTable(factorCount, interactionLevel)
{
}

GradientRegressionCoefficientTable* GradientRegressionCoefficientTable::Create(const int factorCount, const int interactionLevel)
{
    GradientRegressionCoefficientTable *table = new GradientRegressionCoefficientTable(factorCount, interactionLevel);
    table->generateCoeffs();
    return table;
}

void GradientRegressionCoefficientTable::generateCoeffs()
{
    setCoeffs(generateGradientCoeffs(factorCount(), interactionLevel()));
}

void GradientRegressionCoefficientTable::calcNormCoefs(const IFactorTable &xTable, const IResponseTable &yTable)
{
    calcGradientNormCoefs(xTable, yTable, *this);
}

void GradientRegressionCoefficientTable::calcNaturalCoefs(const IFactorTable &xTable, const IResponseTable &yTable)
{
    calcGradientNaturalCoefs(xTable, yTable, *this);
}
