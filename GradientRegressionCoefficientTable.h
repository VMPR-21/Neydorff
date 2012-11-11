#ifndef GRADIENTREGRESSIONCOEFFICIENTTABLE_H
#define GRADIENTREGRESSIONCOEFFICIENTTABLE_H

#include "RegressionCoefficientTable.h"

class GradientRegressionCoefficientTable : public RegressionCoefficientTable
{
public:
    //virtual void calculate(const IFactorTable &xTable, const IResponseTable &yTable);//рассчитать на основе таблиц с факторами и откликами.
    //virtual const std::vector<bCoeff>& coeffs() const;//рассчитанные коэффициенты ур-ния регрессии Bi
    //virtual double trusted_value() const;// доверительная вероятность коэффициента уравнения (нормированное значение)
    //virtual std::vector<bCoeff> significant_coeffs() const;// значимые коэффициенты.
    //virtual int count() const;//общее число коэффициентов

    //std::vector<bCoeff>& coeffs();//рассчитанные коэффициенты ур-ния регрессии Bi
    static GradientRegressionCoefficientTable* Create(const int factorCount, const int interactionLevel);

protected:
    virtual void generateCoeffs(); // ф-ция генерации индексов коэфф-тов уравнения регрессии
    virtual void calcNormCoefs(const IFactorTable &xTable, const IResponseTable &yTable); // ф-ция расчета нормированных коэф-тов градиентного уравнения регресии
    // ф-ция расчета натуральных коэф-тов градиентного уравнения регрессии
    virtual void calcNaturalCoefs(const IFactorTable &xTable, const IResponseTable &yTable);

private:
    GradientRegressionCoefficientTable(const int factorCount, const int interactionLevel);
};

#endif // GRADIENTREGRESSIONCOEFFICIENTTABLE_H
