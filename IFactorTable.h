#ifndef IFACTORTABLE_H
#define IFACTORTABLE_H

#include <vector>
#include "DataStream.h"
#include <QStringList>
#include <QTextStream>

// интерфейс таблицы факторов
class IFactorTable
{
public:

    virtual int count() const = 0;      // кол-во факторов
    virtual int rowCount() const = 0;   // кол-во строк
    virtual double norm_at(int number, int row) const = 0;      // получить нормированное значение ячейки таблицы. number - № фактора. row - № строки.
    virtual double natural_at(int number, int row) const = 0;   // получить натуральное значение ячейки таблицы

    virtual bool supportsReplica() const = 0;             // поддерживает ли реплики
    virtual bool isGeneral(int factor_number) const = 0;  // является ли factor_number фактор основным
    virtual int generalFactorCount() const = 0;           // кол-во основных факторов
    virtual void setGenerateRelation(int number, const std::vector<int> &relation) = 0;  // установить генерирующее соотношение
    virtual std::vector<int> getGenerateRelation(int number) const = 0;

    // интервалы варьирования
    virtual double xDelta(int number) const = 0;
    virtual void setXdelta(int number, double d) = 0;
    // центра плана
    virtual double xCenter(int number) const = 0;
    virtual void setXcenter(int number, double c) = 0;

    //описания факторов
    virtual QStringList factorsDescriptions() const = 0;
    virtual void setFactorsDescriptions(const QStringList &descriptions) = 0;

    //загрузка и сохранение в поток данных таблицы
    //для помещения в поток и извлечения из потока использовать операторы >> и <<
    virtual void loadFrom(DataStream &srcStream) = 0;
    virtual void saveTo(DataStream &dstStream) = 0;

    //загрузка и сохранение в CSV
    virtual void loadFromCSV(QTextStream &srcStream) = 0;
    virtual void saveToCSV(QTextStream &dstStream) = 0;

    virtual int getInteractionLevel() = 0;
    virtual void setInteractionLevel(int interactionLevel) = 0;

    virtual ~IFactorTable() {}

    // удаляем фактор
    virtual void Remove(int Xnumber)=0;
};

#endif // FACTORTABLE_H
