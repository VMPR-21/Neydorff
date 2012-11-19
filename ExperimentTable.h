#ifndef EXPERIMENTTABLE_H
#define EXPERIMENTTABLE_H

#include "IExperimentTable.h"
#include "IFactorTable.h"
#include "IResponseTable.h"
#include "IRegressionCoefficientTable.h"
#include "TableType.h"

//Основная таблица эксперимента. Содержит в себе все необходимое.
class ExperimentTable : public IExperimentTable
{
public:
    //реализация IExperimentTable
    virtual IFactorTable& x() const;
    virtual IResponseTable& y() const;
    virtual IRegressionCoefficientTable& b() const;

    virtual int factorCount() const;
    virtual int rowCount() const;

    virtual void statisticProcessing(double trust_probability, ICriteria &criteria, CenterYDelegatePtr ysRequestDel,bool isFormulaModel);
    virtual void coeffsProcessing(double trust_probability);
    virtual double evalFuncValue(std::vector<double> xvals) const;

    static QString doubleWithComma(double d);
    static QString doubleWithDot(QString d);

    //ExperimentTable(int factor_count, int row_count, int replica_delimiter);
    static IExperimentTable* createExperimentTable(TableType tableType, int factor_count, int replica_delimiter, int interaction_level, QString displeyFunction = "", QString cornerMeasure = "");
    virtual ~ExperimentTable();

    bool load(const char *fileName);
    bool save(const char *fileName);

    void saveEvaluateFunction(DataStream &srcStream);
    void loadEvaluateFunction(DataStream &srcStream);
    void loadEvaluateFunctionFromCSV(QTextStream &srcStream);
    void saveEvaluateFunctionToCSV(QTextStream &srcStream);

    void saveExtrToCSV(QTextStream &srcStream);
    void loadExtrFromCSV(QTextStream &srcStream);

    void setEvaluateFunction(QString evaluate, QString measure);
    QStringList getEvaluateFunction();

    bool getIsMax() const;
    double getStrideParameter() const;
    int getNumberStride() const;
    double getInterestAllowedDeviation() const;

    void setIsMax(bool isMax) const;
    void setStrideParameter(double strideParameter) const;
    void setNumberStride(int numberStride) const;
    void setInterestAllowedDeviation(double interestAllowedDeviation) const;

protected:
    //фабричные методы, создающие конкретные экземпляры необходимых объектов.
    //virtual IFactorTable* createFactorTable(int factor_count, int row_count, int replica_delimiter) = 0;
    //virtual IResponseTable* createResponseTable(int row_count) = 0;
    //virtual IRegressionCoefficientTable* createRegressionCoeffsTable() = 0;

private:
    IFactorTable *_xTable;
    IResponseTable *_yTable;
    IRegressionCoefficientTable *_bTable;
    QStringList evaluateFunction;

    // От Коли
    mutable bool isMax;
    mutable double strideParameter; //h
    mutable int numberStride; //ch
    mutable double interestAllowedDeviation; //dev
    // От Коли

    ExperimentTable();
    static IExperimentTable* createCentralCompositeExperimentTable(int factor_count, int interaction_level, QString displeyFunction = "", QString cornerMeasure = "");
    static IExperimentTable* createGradientExperimentTable(int factor_count, int replica_delimiter, int interaction_level, QString displeyFunction = "", QString cornerMeasure = "");
};

#endif // EXPERIMENTTABLE_H
