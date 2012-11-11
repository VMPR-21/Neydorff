#ifndef BTABLE_H
#define BTABLE_H

#include <vector>
#include "DataStream.h"
#include <QTextStream>

class IFactorTable;
class IResponseTable;
class bCoeff;

// класс-интерфейс таблицы коэффициентов регрессии
class IRegressionCoefficientTable
{
public:

    //рассчитать на основе таблиц с факторами и откликами.
    virtual void calculate(const IFactorTable &xTable, const IResponseTable &yTable, const double trusted_probability) = 0;
    virtual const std::vector<bCoeff>& coeffs() const = 0;  //рассчитанные коэффициенты ур-ния регрессии Bi
    virtual double trusted_value() const = 0;// доверительная вероятность коэффициента уравнения (нормированное значение)
    virtual std::vector<bCoeff> significant_coeffs() const = 0;// значимые коэффициенты.
    virtual int count() const = 0;//общее число коэффициентов
    virtual int interactionLevel() const = 0; // уровень взаимодействия факторов

    virtual void changeInteractionLevel(int newLevel) = 0; // изменение interactionLevel.

    virtual int factorCount() const = 0;    // кол-во факторов
    virtual bCoeff* at(const std::vector<int> &index) = 0;  // получение коэффициента по индексу

    //загрузка и сохранение в поток данных таблицы
    //для помещения в поток и извлечения из потока использовать операторы >> и <<
    virtual void loadFrom(DataStream &srcStream) = 0;
    virtual void saveTo(DataStream &dstStream) = 0;

    //загрузка и сохранение в CSV
    virtual void loadFromCSV(QTextStream &srcStream) = 0;
    virtual void saveToCSV(QTextStream &dstStream) = 0;

    virtual ~IRegressionCoefficientTable() {}
};

#endif // BTABLE_H
