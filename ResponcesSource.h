#ifndef ResponcesSource_H
#define ResponcesSource_H

#include "IResponcesSource.h"
#include "QStringList"

class ResponcesSource: public IResponcesSource
{
public:
    ResponcesSource();
    ResponcesSource(ResponcesSource &src):
        FactNum(src.FactNum), PFEnum(src.PFEnum), DrobRepl(src.DrobRepl),
        ActualFactNum(src.ActualFactNum), MinFactNum(src.MinFactNum),
        MaxDrobRepl(src.MaxDrobRepl), parall(src.parall), FactValues(src.FactValues),
        FactDivergences(src.FactDivergences), Descriptions(src.Descriptions) {    }
    ResponcesSource& operator=(const IResponcesSource& src)
    {
        FactNum = ((ResponcesSource&)src).FactNum;
        PFEnum = ((ResponcesSource&)src).PFEnum;
        DrobRepl = ((ResponcesSource&)src).DrobRepl;
        ActualFactNum = ((ResponcesSource&)src).ActualFactNum;
        MinFactNum = ((ResponcesSource&)src).MinFactNum;
        MaxDrobRepl = ((ResponcesSource&)src).MaxDrobRepl;
        FactValues = ((ResponcesSource&)src).FactValues;
        FactDivergences = ((ResponcesSource&)src).FactDivergences;
        parall = ((ResponcesSource&)src).parall;
        Descriptions = ((ResponcesSource&)src).Descriptions;
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
};

#endif // ResponcesSource_H
