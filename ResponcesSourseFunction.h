#ifndef RESPONCESSOURSEFUNCTION_H
#define RESPONCESSOURSEFUNCTION_H

#include "IResponcesSource.h"
#include <QStringList>
#include <QtScript/QScriptEngine>
#include <QtScript/QtScript>
#include <QMessageBox>
#include <QtXml/QtXml>
#include <QtXml/QXmlStreamReader>
#include <QtXml/QXmlStreamWriter>

#define PRINT_STRING 0
#define EVALUATE_STRING 1


class ResponcesSourseFunction: public IResponcesSource
{
public:
    ResponcesSourseFunction();
    ~ResponcesSourseFunction();
    ResponcesSourseFunction(ResponcesSourseFunction &src):
        FactNum(src.FactNum), PFEnum(src.PFEnum), DrobRepl(src.DrobRepl),
        ActualFactNum(src.ActualFactNum), MinFactNum(src.MinFactNum),
        MaxDrobRepl(src.MaxDrobRepl), parall(src.parall), FactValues(src.FactValues),
        FactDivergences(src.FactDivergences), Descriptions(src.Descriptions),
        EvaluateFunction(src.EvaluateFunction) {    }

    ResponcesSourseFunction& operator=(const IResponcesSource& src)
    {
        FactNum = ((ResponcesSourseFunction&)src).FactNum;
        PFEnum = ((ResponcesSourseFunction&)src).PFEnum;
        DrobRepl = ((ResponcesSourseFunction&)src).DrobRepl;
        ActualFactNum = ((ResponcesSourseFunction&)src).ActualFactNum;
        MinFactNum = ((ResponcesSourseFunction&)src).MinFactNum;
        MaxDrobRepl = ((ResponcesSourseFunction&)src).MaxDrobRepl;
        FactValues = ((ResponcesSourseFunction&)src).FactValues;
        FactDivergences = ((ResponcesSourseFunction&)src).FactDivergences;
        parall = ((ResponcesSourseFunction&)src).parall;
        Descriptions = ((ResponcesSourseFunction&)src).Descriptions;
        this->EvaluateFunction = ((ResponcesSourseFunction&)src).EvaluateFunction;
        return *this;
    }

    //получить центр плана для указаного № фактора
    double centerFor(int xIndex);
    // возвращает интервал вырьирования для указаного № фактора
    void intervalFor(int xIndex, double *xMin, double *xMax);
    //получить все || значения опытов для указаных нормальных значений факторов   xVal для каждого row -> getData -> получаю Y с которыми работаю
    std::vector<double> getYdata(std::vector<int> normCords);

    std::vector<double> getYdata2(std::vector<double> normCords);

    int inputsCount() const;//общее число факторов
    int actualInputsCount() const;//число "базовых", основных факторов. Остальные - комбинация базовых.

    QStringList getDescriptions() const;

    //задать формулу для вычислений
    void SetEvaluateFunction(QString DisplayFunction,QString EvaluateFunction)
    {
        this->EvaluateFunction.clear();
        this->EvaluateFunction << DisplayFunction<<EvaluateFunction;
    }
    //взять формулу для вычислений
    QStringList getEvaluateFunction()
    {
        return this->EvaluateFunction;
    }

    int FactNum;
    int PFEnum;
    double DrobRepl;
    int ActualFactNum;
    int MinFactNum;
    double MaxDrobRepl;
    double parall;
    std::vector<double> FactValues;
    std::vector<double> FactDivergences;
    QStringList Descriptions;

    // функция введённая юзвером
    //QString EvaluateFunction;
    QStringList EvaluateFunction; //0 - display function
                                    //1- evaluate function


    // интерпретатор ecmascript
    //QScriptEngine* RunScript;
    // результат вычислений
    //QScriptValue ScriptRes;

    // используется если формула содержит тригонометрические функции , значения для которых указанны в градусы
    QString CheckFormulaForTrinometricFunctions(QString text);

    // получение параметров тригонометрической функции
    QString GetTrigonometricFuncParams(QString text);
    QString CheckExpressionPattern(QString pattern, QString text);

    bool LoadFormula(QString FileName);
    bool SaveFormula(QString FileName);

private: bool RecognizeDomChildInfo(QDomElement ch1,int Parrent=0);

};

#endif // RESPONCESSOURSEFUNCTION_H
