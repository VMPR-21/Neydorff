
#ifndef FACTORTABLE_H
#define FACTORTABLE_H

#include <vector>
#include "IFactorTable.h"

//базовый класс для таблиц факторов(ЦКП, 2х уровневый т т.д.)
class FactorTable : public IFactorTable
{
public:
    // реализация IFactorTable
    virtual int count() const;
    virtual int rowCount() const;
    virtual double norm_at(int number, int row) const;
    virtual double natural_at(int number, int row) const;

    double   xDelta(int number) const;
    double  xCenter(int number) const;
    void  setXdelta(int number, double d);
    void setXcenter(int number, double c);
    void Remove(int Xnumber);

    QStringList factorsDescriptions() const;
    void setFactorsDescriptions(const QStringList &descriptions);

    //создание с указаным числом факторов-иксов и числом строк таблицы соответственно.
    FactorTable(int factor_count, int row_count);
    virtual ~FactorTable();

    int getInteractionLevel();
    void setInteractionLevel(int interactionLevel);

    void loadFrom(DataStream &srcStream);
    void saveTo(DataStream &dstStream);

    void loadFromCSV(QTextStream &srcStream);
    void saveToCSV(QTextStream &dstStream);

protected:
    void set_at(int factor, int row, double value);
    std::vector<std::vector<double>* > *_x;   // вектор для хранения таблицы
    std::vector<double> *_center;             // центры плана факторов
    std::vector<double> *_delta;              // центры плана факторов

    QStringList _descriptions;
    int _interactionLevel;
private:

};

#endif // FACTORTABLE_H

