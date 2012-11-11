#ifndef STUDENTCRITERIA_H
#define STUDENTCRITERIA_H
#include "vector"
#include "ICriteria.h"

using namespace std;


// Реализация таблицы значений критерия Стьюдента
class StudentCriteria
{
private:
    static bool initialized;
    static vector<int> nVals;
    static vector<double> m_supportedProbability;
    static vector<double> m_supportedProbabilityInPercent;
    static vector<double> probTable;
    static void _init();
    static void init()
    {
        if(!StudentCriteria::initialized)
            StudentCriteria::_init();
    }
public:
    // Получить табличное значения для критерия для n-степеней свободы и доверительной вероятности p
    static double GetStudentVal(int n, double p);
    // Поддерживаемые доверительные интервалы
    static vector<double> GetSupportedProbability();
    // Поддерживаемые доверительные вероятности
    static vector<double> GetSupportedProbabilityInPercent();
};



#endif // STUDENTCRITERIA_H
