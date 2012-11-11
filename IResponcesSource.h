#ifndef IDATASOURCE_H
#define IDATASOURCE_H

#include <vector>
#include "QStringList"
#include <QTextStream>

//Абстрактный источник данных(откликов) для экспериментов.
class IResponcesSource
{
public:

    //получить центр плана для указаного № фактора
    virtual double centerFor(int xIndex) = 0;
    // возвращает интервал вырьирования для указаного № фактора
    virtual void intervalFor(int xIndex, double *xMin, double *xMax) = 0;
    //получить все || значения опытов для указаных нормальных значений факторов
    virtual std::vector<double> getYdata(std::vector<int> normCords) = 0;
    virtual std::vector<double> getYdata2(std::vector<double> normCords) = 0;

    virtual int inputsCount() const = 0; //общее число факторов
    virtual int actualInputsCount() const = 0; //число "базовых", основных факторов. Остальные - комбинация базовых.

    //текстовые описания факторов
    virtual QStringList getDescriptions() const = 0;

    //получить информацию о функции
    virtual QStringList getEvaluateFunction() = 0;

    virtual ~IResponcesSource() {}
};

#endif // IDATASOURCE_H
