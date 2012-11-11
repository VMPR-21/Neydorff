#ifndef BARTLETECRITERIA_H
#define BARTLETECRITERIA_H
#include "vector"

using namespace std;

class BartleteCriteria
{
    static bool initialized;
    static vector<int> nVals;
    static vector<double> m_probs;
    static vector<double> probTable;
    static void _init();
    static void init()
    {
        if(!BartleteCriteria::initialized)
            BartleteCriteria::_init();
    }
public:
    BartleteCriteria();
    static double GetCriteriaVal(int n, double p);
    static vector<double> SupportedProbs();
};

#endif // BARTLETECRITERIA_H
