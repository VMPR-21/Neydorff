#ifndef IEXPERIMENTTABLE_H
#define IEXPERIMENTTABLE_H

#include <vector>
#include "Experementator.h"

class ICriteria;
class IFactorTable;
class IResponseTable;
class IRegressionCoefficientTable;
#include "IResponseTable.h"

// интерфейс таблицы эксперимента
class IExperimentTable
{
public:
    virtual IFactorTable& x() const = 0;//факторы (иксы)
    virtual IResponseTable& y() const = 0;//отклики
    virtual IRegressionCoefficientTable& b() const = 0; //коэф-ты уравнения регрессии Bi

    virtual int factorCount() const = 0;//число факторов-иксов в таблице
    virtual int rowCount() const = 0;//число строк с опытами в таблице

    virtual void setEvaluateFunction(QString evaluate, QString measure) = 0;
    virtual QStringList getEvaluateFunction() = 0;

    // стат. обработка с использованием trust_probability пороговой доверительной вероятноси и criteria критерием(м. быть Стьюдента, Фишера и т.д.).
    /* update: isFormulaModel - новый эксперимент = формула*/
    virtual void statisticProcessing(double trust_probability, ICriteria &criteria, CenterYDelegatePtr ysRequestDel,bool isFormulaModel) = 0;

    //Вычисление коэффициентов уравнения регрессии (?)
    virtual void coeffsProcessing(double trust_probability) = 0;

    //вычислить значение функции для указаных аргументов. Для этого должны быть известны коэф-ты Bi.
    virtual double evalFuncValue(std::vector<double> xvals) const = 0;

    //загрузка и сохранение в файл содержимого таблицы.
    virtual bool load(const char *fileName) = 0;
    virtual bool save(const char *fileName) = 0;
    virtual bool save(const char *fileName) const = 0;

    // От Коли
    virtual bool getIsMax() const = 0;
    virtual double getStrideParameter() const = 0;
    virtual int getNumberStride() const = 0;
    virtual double getInterestAllowedDeviation() const = 0;

    virtual void setIsMax(bool isMax) const = 0;
    virtual void setStrideParameter(double strideParameter) const = 0;
    virtual void setNumberStride(int numberStride) const = 0;
    virtual void setInterestAllowedDeviation(double interestAllowedDeviation) const = 0;
    virtual void setExperimentPoint(std::vector<ExperimentPoint> v) const = 0;

    virtual ~IExperimentTable() {}
};

#endif // EXPERIMENTTABLE_H
