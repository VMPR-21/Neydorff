#ifndef KOHRENCRITERIA_H
#define KOHRENCRITERIA_H

#include "vector"

using namespace std;

class KohrenCriteria
{
public:
    static bool initialized;
    static vector<int> freedoms;
    static vector<int> nCounts;
    static vector<double> probTable;
    static void _init();
    static void init()
    {
        if(!KohrenCriteria::initialized)
            KohrenCriteria::_init();
    }
public:
    static double GetCriteriaVal(int N, int f);
    KohrenCriteria();
    static vector<double> SupportedProbs();
};

#endif // KOHRENCRITERIA_H
