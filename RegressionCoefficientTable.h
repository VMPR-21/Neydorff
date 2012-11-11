#ifndef REGRESSIONCOEFFICIENTTABLE_H
#define REGRESSIONCOEFFICIENTTABLE_H

#include "IRegressionCoefficientTable.h"
#include <vector>
#include "TableType.h"
class bCoeff;
class IFactorTable;
class IResponseTable;

class RegressionCoefficientTable : public IRegressionCoefficientTable
{
public:
    static IRegressionCoefficientTable* Create(TableType tableType, const int factorCount, const int interactionLevel);    // фабрика по созданию таблиц
    virtual void calculate(const IFactorTable &xTable, const IResponseTable &yTable, const double trusted_probability);//рассчитать на основе таблиц с факторами и откликами.
    virtual const std::vector<bCoeff>& coeffs() const;//рассчитанные коэффициенты ур-ния регрессии Bi
    virtual double trusted_value() const;// доверительная вероятность коэффициента уравнения (нормированное значение)
    virtual std::vector<bCoeff> significant_coeffs() const;// значимые коэффициенты.
    virtual int count() const;//общее число коэффициентов
    virtual int interactionLevel() const; // уровень взаимодействия факторов

    virtual void changeInteractionLevel(int newLevel);

    virtual int factorCount() const;    // кол-во факторов
    virtual bCoeff* at(const std::vector<int> &index);

    std::vector<bCoeff>& coeffs();//рассчитанные коэффициенты ур-ния регрессии Bi

    virtual void loadFrom(DataStream &srcStream);
    virtual void saveTo(DataStream &dstStream);

    virtual void loadFromCSV(QTextStream &srcStream);
    virtual void saveToCSV(QTextStream &dstStream);

private:
    double _trustedValue;
    std::vector<bCoeff> *_coefficients;
    int _factorCount;
    int _interactionLevel;

    void calculate_trusted_value(const IResponseTable &yTable, double trusted_probability);

protected:
    void setCoeffs(std::vector<bCoeff> *value);
    RegressionCoefficientTable(const int factorCount, const int interactionLevel);
    virtual void generateCoeffs() = 0; // ф-ция генерации индексов коэфф-тов уравнения регрессии
    virtual void calcNormCoefs(const IFactorTable &xTable, const IResponseTable &yTable) = 0; // ф-ция расчета нормированных коэф-тов градиентного уравнения регресии
    // ф-ция расчета натуральных коэф-тов градиентного уравнения регрессии
    virtual void calcNaturalCoefs(const IFactorTable &xTable, const IResponseTable &yTable) = 0;
};

#endif // REGRESSIONCOEFFICIENTTABLE_H
