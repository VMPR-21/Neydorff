#ifndef GRADIENTFACTORTABLE_H
#define GRADIENTFACTORTABLE_H

#include "FactorTable.h"

//Основная таблица иксов-факторов.
class GradientFactorTable : public FactorTable
{
public:
    // реализация IFactorTable
    virtual bool supportsReplica() const;             // поддерживает ли реплики
    virtual bool isGeneral(int factor_number) const;  // является ли фактор основным
    virtual int generalFactorCount() const;           // кол-во основных факторов
    virtual void setGenerateRelation(int number, const std::vector<int> &relation);  // установить генерирующее соотношение
    virtual std::vector<int> getGenerateRelation(int number) const;

    //создание таблицы. replica_delimiter - дробность реплики. 1 - ПФЭ.
    GradientFactorTable(int factor_count, int row_count, int replica_delimiter);
    virtual ~GradientFactorTable();

    void loadFrom(DataStream &srcStream);
    void saveTo(DataStream &dstStream);

    void loadFromCSV(QTextStream &srcStream);
    void saveToCSV(QTextStream &dstStream);
private:
    int _replicaDelimiter;
    int _factorCount;
    int _generalFactorCount;
    int _replica_row_count;
    void generateFullExperimentCoeffs();    // метод генерации таблицы факторов полного факторного эксперимента
    std::vector<std::vector<int> > _generateRelations;
};

#endif // GRADIENTFACTORTABLE_H
