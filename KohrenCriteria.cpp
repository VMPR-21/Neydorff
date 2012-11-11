#include "KohrenCriteria.h"
#include "cmath"
#include "assert.h"
#include <stdlib.h> // abs(int)
bool KohrenCriteria::initialized;
vector<int> KohrenCriteria::freedoms;
vector<int> KohrenCriteria::nCounts;
vector<double> KohrenCriteria::probTable;

KohrenCriteria::KohrenCriteria()
{
}

void KohrenCriteria::_init()
{
    int mas[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    KohrenCriteria::freedoms.assign(mas, mas + (sizeof(mas) / sizeof(mas[0])));

    int msv[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 15, 20 };
    KohrenCriteria::nCounts.assign(msv, msv + (sizeof(msv) / sizeof(msv[0])));

    double probs[] =
    {
        /*        1       2       3       4       5       6        7      8  */
        /*2*/   0.999,	0.975,	0.939,	0.906,	0.877,	0.853,	0.833,	0.816,
        /*3*/	0.967,	0.871,	0.798,	0.746,	0.707,	0.677,	0.653,	0.633,
        /*4*/	0.907,	0.768,	0.684,	0.629,	0.59,	0.56,	0.637,	0.518,
        /*5*/	0.841,	0.684,	0.598,	0.544,	0.507,	0.478,	0.456,	0.439,
        /*6*/	0.781,	0.616,	0.531,	0.48,	0.445,	0.418,	0.398,	0.382,
        /*7*/	0.727,	0.561,	0.48,	0.431,	0.397,	0.373,	0.354,	0.338,
        /*8*/	0.68,	0.516,	0.438,	0.391,	0.36,	0.336,	0.319,	0.304,
        /*9*/	0.639,	0.478,	0.403,	0.358,	0.329,	0.307,	0.29,	0.277,
        /*10*/	0.602,	0.445,	0.373,	0.331,	0.303,	0.282,	0.267,	0.254,
        /*12*/	0.541,	0.392,	0.326,	0.288,	0.262,	0.244,	0.23,	0.219,
        /*15*/	0.471,	0.335,	0.276,	0.242,	0.22,	0.203,	0.191,	0.182,
        /*20*/	0.389,	0.271,	0.221,	0.192,	0.174,	0.16,	0.15,	0.142

    };

    KohrenCriteria::probTable.assign(probs, probs + (sizeof(probs) / sizeof(probs[0])));
    initialized = true;
}

double KohrenCriteria::GetCriteriaVal(int N, int f)
{
    assert(N > 1);

    if(N > 20) N = 20;

    KohrenCriteria::init();

    int indexN = 0;
    double min = abs(nCounts[0] - N);

    for(size_t i = 0; i < nCounts.size(); i++)
    {
        if(min > abs(nCounts[i] - N))
        {
            min = abs(nCounts[i] - N);
            indexN = i;
        }
    }

    assert(indexN != -1);

    int indexF = 0;
    min = abs(freedoms[0] - f);

    for(size_t i = 0; i < freedoms.size(); i++)
    {
        if(min > abs(freedoms[i] - f))
        {
            min = abs(freedoms[i] - f);
            indexF = i;
        }
    }

    assert(indexF != -1);

    return probTable[indexN * freedoms.size() + indexF];
}

vector<double> KohrenCriteria::SupportedProbs()
{
    vector<double> vec;
    vec.push_back(0.95);
    return vec;
}
