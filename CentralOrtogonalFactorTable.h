#ifndef CENTRALORTOGONALFACTORTABLE_H
#define CENTRALORTOGONALFACTORTABLE_H

#include "FactorTable.h"

class CentralOrtogonalFactorTable : public FactorTable
{
public:
    CentralOrtogonalFactorTable(int factor_count, int row_count);
    virtual ~CentralOrtogonalFactorTable();

    virtual bool supportsReplica() const;             // поддерживает ли реплики
    virtual bool isGeneral(int factor_number) const;  // является ли factor_number фактор основным
    virtual int generalFactorCount() const;           // кол-во основных факторов
    virtual void setGenerateRelation(int number, const std::vector<int> &relation);  // установить генерирующее соотношение
    virtual std::vector<int> getGenerateRelation(int number) const;
};

#endif // CENTRALORTOGONALFACTORTABLE_H
