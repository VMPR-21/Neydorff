#include "ResponcesSource.h"

ResponcesSource::ResponcesSource(): FactNum(0), PFEnum(0), ActualFactNum(0), MinFactNum(0)
{
}

double ResponcesSource::centerFor(int xIndex)
{
    return FactValues.at(xIndex);
}

// возвращает интервал вырьирования для указаного № фактора
void ResponcesSource::intervalFor(int xIndex, double *xMin, double *xMax)
{
    *xMin = FactDivergences.at(xIndex);
    *xMax = FactDivergences.at(xIndex);
}

//получить все || значения опытов для указаных нормальных значений факторов xVal для каждого row -> getData -> получаю Y с которыми работаю
std::vector<double> ResponcesSource::getYdata(std::vector<int> normCords)
{
    return std::vector<double>(parall);
    //return std::vector<double>();
}

std::vector<double> ResponcesSource::getYdata2(std::vector<double> normCords)
{
    return std::vector<double>(parall);
    //return std::vector<double>();
}

int ResponcesSource::inputsCount() const
{
    return FactNum;
}

//общее число факторов
int ResponcesSource::actualInputsCount() const
{
    return ActualFactNum;
}
//число "базовых", основных факторов. Остальные - комбинация базовых.

QStringList ResponcesSource::getDescriptions() const
{
    return Descriptions;
}
