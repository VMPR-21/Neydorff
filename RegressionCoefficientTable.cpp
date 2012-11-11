#include "RegressionCoefficientTable.h"
#include <assert.h>
#include <vector>
#include <limits>
#include <cmath>
#include "bCoeff.h"
#include "IResponseTable.h"
#include "StudentCriteria.h"
#include "GradientRegressionCoefficientTable.h"
#include "CentralOrtogonalRegressionCoefficientTable.h"
#include <QStringList>
#include "ExperimentTable.h"

#ifndef WIN32 //for better linux compability
#define _isnan isnan
#endif

using namespace std;

IRegressionCoefficientTable* RegressionCoefficientTable::Create(TableType tableType, const int factorCount, const int interactionLevel)
{
    IRegressionCoefficientTable *table = NULL;

    switch(tableType)
    {
    case FullFactorGradient:
    case ReplicaGradient:
        table = GradientRegressionCoefficientTable::Create(factorCount, interactionLevel);
        break;

    case CentralOrtogonalComposite:
        table = CentralOrtogonalRegressionCoefficientTable::Create(factorCount, interactionLevel);
        break;
    }

    assert(table != NULL);
    return table;
}

// конструктор (кол-во факторов, уровень взаимодействия)
RegressionCoefficientTable::RegressionCoefficientTable(const int factorCount, const int interactionLevel)
{
    assert(factorCount > 0);
    assert(interactionLevel > 0);
    assert(interactionLevel < 8);

    _trustedValue = std::numeric_limits<double>::quiet_NaN();

    _factorCount = factorCount;
    _interactionLevel = interactionLevel;
    _coefficients = NULL;
}


int RegressionCoefficientTable::count() const
{
    //assert(_coefficients != NULL);
    if(!_coefficients)
        return 0;

    return _coefficients->size();
}

int RegressionCoefficientTable::interactionLevel() const
{
    return _interactionLevel;
}

void RegressionCoefficientTable::changeInteractionLevel(int newLevel)
{
    _interactionLevel = newLevel;
    generateCoeffs();
}

int RegressionCoefficientTable::factorCount() const
{
    return _factorCount;
}


const std::vector<bCoeff>& RegressionCoefficientTable::coeffs() const
{
    assert(_coefficients != NULL);
    return *_coefficients;
}

std::vector<bCoeff>& RegressionCoefficientTable::coeffs()
{
    assert(_coefficients != NULL);
    return *_coefficients;
}

double RegressionCoefficientTable::trusted_value() const
{
    assert(!::_isnan(_trustedValue));
    return _trustedValue;
}

std::vector<bCoeff> RegressionCoefficientTable::significant_coeffs() const
{
    assert(_coefficients != NULL);

    vector<bCoeff> trusted;

    for(size_t i = 0; i < _coefficients->size(); i++)
    {
        if(_coefficients->at(i).is_significant())
        {
            bCoeff _copy(_coefficients->at(i).index(),
                         _coefficients->at(i).norm_value(),
                         _coefficients->at(i).natural_value(),
                         true);
            trusted.push_back(_copy);
        }
    }

    return trusted;
}

bCoeff* RegressionCoefficientTable::at(const std::vector<int> &index)
{
    assert(_coefficients != NULL);

    for(size_t coeffNumber = 0; coeffNumber < _coefficients->size(); coeffNumber++)
    {
        if(index == _coefficients->at(coeffNumber).index())
            return &_coefficients->at(coeffNumber);

        //        if(_coefficients->at(coeffNumber).index().size() != index.size())
        //            continue;

        //        for(size_t digitNumber = 0; digitNumber < index.size(); digitNumber++)
        //            if(_coefficients->at(coeffNumber).index()[digitNumber] != index[digitNumber])
        //                break;
        //        return &_coefficients->at(coeffNumber);
    }

    return NULL;
}

/*
//расчитать список индексов значимых коэф-тов ур-ия регр-ии
std::vector<int> goodBindexes(IRegressionCoefficientTable &bTable, double probability)
{
    double sko_b = 0.0;
    for(int i = 0; i < table.rows(); i++)
        sko_b += table.disp(i);
    sko_b /= table.rows() * table.rows();
    sko_b = sqrt(sko_b);

    // получение табличного значения критерия
    double critical = CriteriaTable::getStudentVal(table.bCount(), probability);

    double delta_b = critical * sko_b;

    vector<int> result;
    for(int i = 0; i < table.bCount(); i++)
        if(fabs(table.b(i) >= delta_b))
            result.push_back(i);

    return result;
}
*/

// расчет доверительного интервала для коэф-тов уравнения
void RegressionCoefficientTable::calculate_trusted_value(const IResponseTable &yTable, double trusted_probability)
{
    assert(trusted_probability > 0.0 && trusted_probability < 1.0);
    double sko_b = sqrt(yTable.dispersionVospr() / yTable.count());

    // получение табличного значения критерия
    double critical = StudentCriteria::GetStudentVal(count(), trusted_probability);
    _trustedValue = critical * sko_b;
}

/*
//расчитать список индексов значимых коэф-тов ур-ия регр-ии
std::vector<bCoeff> RegressionCoefficientTable::significant_coeffs() const
{
    assert(!_isnan(_trustedValue));

    vector<bCoeff> result;
    for(int i = 0; i < bTable.count(); i++)
        if(fabs(bTable.at(i) >= _trustedValue))
            result.push_back(bTable.at(i));

    return result;
}
*/

void RegressionCoefficientTable::calculate(const IFactorTable &xTable, const IResponseTable &yTable, const double trusted_probability)
{
    //_coefficients = generateCoeffs(_factorCount, _interactionLevel);  // сгенерировать коэффициенты по входным данным
    calcNormCoefs(xTable, yTable);                  // считать нормированные коэф-ты
    calculate_trusted_value(yTable, trusted_probability);   // считать доверительный интервал для коэффициентов

    // подготовить натуральные значения коэф-тов регрессии
    for(int coeffNumber = 0; coeffNumber < count(); coeffNumber++)
    {
        // выделить значимые и незначимые коэф-ты
        coeffs()[coeffNumber].set_is_significant(fabs(coeffs()[coeffNumber].norm_value()) >= trusted_value());
        coeffs()[coeffNumber].set_natural_value(0.0);   // обнулить натуральные значения коэф-тов
    }

    calcNaturalCoefs(xTable, yTable);  // считать натуральные значения коэф-тов
}

void RegressionCoefficientTable::setCoeffs(std::vector<bCoeff> *value)
{
    assert(value != NULL);
    _coefficients = value;
}

void RegressionCoefficientTable::loadFrom(DataStream &srcStream)
{
    srcStream >> _trustedValue;
    srcStream >> _factorCount;
    srcStream >> _interactionLevel;

    _coefficients = new std::vector<bCoeff>();
    quint64 size;
    srcStream >> size;

    for(size_t i = 0; i < size; i++)
    {
        quint64 idxSize;
        srcStream >> idxSize;
        std::vector<int> index(idxSize);

        for(size_t j = 0; j < idxSize; j++)
        {
            int v;
            srcStream >> v;
            index.at(j) = v;
        }

        double normVal, natVal;
        srcStream >> normVal;
        srcStream >> natVal;
        bool isSignificant;
        srcStream >> isSignificant;

        _coefficients->push_back(bCoeff(index, normVal, natVal, isSignificant));
    }
}

void RegressionCoefficientTable::loadFromCSV(QTextStream &srcStream)
{
    QString tmp;
    QStringList tmpList;
    srcStream.readLine();
    tmp = srcStream.readLine();
    if ("" != tmp) {
        tmp = ExperimentTable::doubleWithDot(tmp.split(QRegExp(";"))[1]);
        _trustedValue = tmp.toDouble();
        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        _factorCount = tmp.toDouble();
        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        _interactionLevel = tmp.toDouble();

        _coefficients = new std::vector<bCoeff>();
        quint64 size, idxSize;
        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        size = tmp.toULongLong();

        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        idxSize = tmp.toULongLong();

        srcStream.readLine();

        for(size_t i = 0; i < size; i++)
        {
            tmpList = srcStream.readLine().split(QRegExp(";"));
            std::vector<int> index(tmpList.length() - 2);
            int j = 0;
            while ("" != tmpList[j + 1]) {
                int v;
                tmp = tmpList[j + 1];
                v = tmp.toInt();
                index.at(j) = v;
                j++;
            }

            double normVal, natVal;
            tmp = ExperimentTable::doubleWithDot(srcStream.readLine().split(QRegExp(";"))[1]);
            normVal = tmp.toDouble();
            tmp = ExperimentTable::doubleWithDot(srcStream.readLine().split(QRegExp(";"))[1]);
            natVal = tmp.toDouble();
            //srcStream >> normVal;
            //srcStream >> natVal;
            bool isSignificant;
            tmp = srcStream.readLine().split(QRegExp(";"))[1];
            isSignificant = tmp.at(0) == '1';
            //srcStream >> isSignificant;

            _coefficients->push_back(bCoeff(index, normVal, natVal, isSignificant));
        }
    }
}

void RegressionCoefficientTable::saveTo(DataStream &dstStream)
{
    dstStream << _trustedValue;
    dstStream << _factorCount;
    dstStream << _interactionLevel;

    if(_coefficients)
        dstStream << (quint64)_coefficients->size();
    else
    {
        dstStream << quint64(0);
        return;
    }

    for(size_t i = 0; i < _coefficients->size(); i++)
    {
        std::vector<int> index = _coefficients->at(i).index();
        dstStream << (quint64)index.size();

        for(size_t j = 0; j < index.size(); j++)
        {
            dstStream << index.at(j);
        }

        double normVal = _coefficients->at(i).norm_value(), natVal = _coefficients->at(i).natural_value();
        bool isSignificant = _coefficients->at(i).is_significant();
        dstStream << normVal;
        dstStream << natVal;
        dstStream << isSignificant;
    }
}

void RegressionCoefficientTable::saveToCSV(QTextStream &dstStream)
{
    dstStream << QString::fromUtf8("ТАБЛИЦА КОЭФФИЦИЕНТОВ РЕГРЕССИИ") << "\r\n";
    dstStream << QString::fromUtf8("Доверительное значение:") << ";" << ExperimentTable::doubleWithComma(_trustedValue) << "\r\n";
    dstStream << QString::fromUtf8("Количество коэффициентов:") << ";";
    if(_coefficients)
        dstStream << (quint64)_coefficients->size();
    else
    {
        dstStream << quint64(0);
        return;
    }
    dstStream << "\r\n";
    dstStream << QString::fromUtf8("КОЭФФИЦИЕНТЫ:") << ";" << QString::fromUtf8("β коэф.:") << ";" << QString::fromUtf8("Натуральное:") << "\r\n";
    for(size_t i = 0; i < _coefficients->size(); i++)
    {
        QString tmp = "";
        std::vector<int> index = _coefficients->at(i).index();
        for(size_t j = 0; j < index.size(); j++)
        {
            tmp += QString::number(index.at(j));
        }
        double normVal = _coefficients->at(i).norm_value(), natVal = _coefficients->at(i).natural_value();
        bool isSignificant = _coefficients->at(i).is_significant();
        dstStream << QString::fromUtf8("β") << tmp << ";" << ExperimentTable::doubleWithComma(normVal);
        dstStream << ";" << ExperimentTable::doubleWithComma(natVal) << "\r\n";
    }
}
