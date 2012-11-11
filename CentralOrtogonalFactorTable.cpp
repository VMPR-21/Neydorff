#include "CentralOrtogonalFactorTable.h"
#include <math.h>
#include <vector>
#include <assert.h>

using namespace std;

CentralOrtogonalFactorTable::CentralOrtogonalFactorTable(int factor_count, int row_count) : FactorTable(factor_count, row_count)
{
    int N0 = pow(2.0, factor_count);      // кол-во опытов ПФЭ
    int N = N0 + 2 * factor_count + 1;  // кол-во опытов в эксперименте
    double g = pow((pow(N * N0, 0.5) - N0) / 2.0, 0.5); // плечо "звездных точек"

    /* заполнить DataTable: индексы, значения */

    // ограничение кол-ва факторов при генерации полного факторного эксперимента
    assert((size_t)factor_count < sizeof(int) * 8);

    // сгенерировать план полного факторного эксперимента
    int max_combination = pow(2.0, factor_count);

    // план полного факторного эксперимента (2^factor_count)
    for(int combination = 0x00; combination < max_combination; combination++)
        for(int fact_num = 0; fact_num < factor_count; fact_num++)
            set_at(fact_num, combination, ((combination >> fact_num) & 0x01) ? 1.0 : -1.0);

    int g_combination = 0x01;

    // заполняем опыты со звездными точками
    for(int row_num = 0; row_num < 2 * factor_count; row_num += 2)
    {
        for(int fact_num = 0; fact_num < factor_count; fact_num++)
        {
            set_at(fact_num, N0 + row_num, ((g_combination >> fact_num) & 0x01) ? -g : 0.0);
            set_at(fact_num, N0 + row_num + 1, ((g_combination >> fact_num) & 0x01) ? g : 0.0);
        }

        g_combination <<= 1;
    }

    // строка - центр плана
    for(int fact_num = 0; fact_num < factor_count; fact_num++)
        set_at(fact_num, N - 1, 0.0);
}

bool CentralOrtogonalFactorTable::supportsReplica() const
{
    return false;   // таблица не поддерживает реплики
}

bool CentralOrtogonalFactorTable::isGeneral(int factor_number) const
{
    assert(factor_number >= 0 && factor_number < count());
    return true;    // все факторы - основные
}

int CentralOrtogonalFactorTable::generalFactorCount() const
{
    return count();  // все факторы таблицы - основные
}

void CentralOrtogonalFactorTable::setGenerateRelation(int number, const std::vector<int> &relation)
{
    assert(0 != 0); // не поддерживается данным типом таблицы
}

std::vector<int> CentralOrtogonalFactorTable::getGenerateRelation(int number) const
{
    assert(0 != 0); // не поддерживается данным типом таблицы
    return std::vector<int>();
}

CentralOrtogonalFactorTable::~CentralOrtogonalFactorTable()
{
}
