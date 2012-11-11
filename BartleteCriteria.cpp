#include "BartleteCriteria.h"
#include "assert.h"
#include "cmath"
#include "algorithm"


bool BartleteCriteria::initialized = false;
vector<int> BartleteCriteria::nVals;
vector<double> BartleteCriteria::m_probs;
vector<double> BartleteCriteria::probTable;

BartleteCriteria::BartleteCriteria()
{
}

void BartleteCriteria::_init()
{
    BartleteCriteria::nVals.reserve(62);
    int mas[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 18, 20, 25, 30 };
    BartleteCriteria::nVals.assign(mas, mas + (sizeof(mas) / sizeof(mas[0])));

    BartleteCriteria::m_probs.reserve(10);
    double msv[] = { 0.99, 0.95, 0.80, 0.20, 0.05, 0.01 };
    BartleteCriteria::m_probs.assign(msv, msv + (sizeof(msv) / sizeof(msv[0])));

    double table[] =
    {
        /* 0.99     0.95    0.80    0.20    0.05   0.01 */
        /*1*/   0.000157, 0.00393, 0.0642,  1.642,  3.841, 6.635,
        /*2*/   0.0201,   0.0103,  0.446,   3.219,  5.991, 9.210,
        /*3*/   0.115,    0.352,   1.005,   4.642,  7.815, 11.345,
        /*4*/   0.297,    0.711,   1.649,   5.980,  9.488, 13.277,
        /*5*/   0.554,    1.145,   2.343,   7.289, 11.070, 15.086,
        /*6*/   0.872,    1.635,   3.070,   8.558, 12.592, 16.812,
        /*7*/   1.239,    2.167,   3.822,   9.803, 14.067, 18.475,
        /*8*/   1.646,    2.733,   4.594,  11.030, 15.507, 20.090,
        /*9*/   2.088,    3.325,   5.380,  12.242, 16.919, 21.666,
        /*10*/   2.558,    3.940,   6.179,  13.442, 18.307, 23.209,
        /*12*/   3.571,    5.226,   7.807,  15.812, 21.026, 26.217,
        /*14*/   4.661,    6.571,   9.467,  18.151, 23.685, 29.141,
        /*16*/   5.812,    7.962,  11.152,  20.465, 26.296, 32.000,
        /*18*/   7.015,    9.390,  12.857,  22.760, 28.869, 34.805,
        /*20*/   8.260,   10.851,  14.578,  25.038, 31.410, 37.566,
        /*25*/  11.524,   14.611,  18.940,  30.675, 37.652, 44.314,
        /*30*/  14.953,   18.493,  23.364,  36.250, 43.773, 50.892
    };

    BartleteCriteria::probTable.assign(table, table + (sizeof(table) / sizeof(table[0])));
    initialized = true;
}

double BartleteCriteria::GetCriteriaVal(int n, double p)
{
    assert(n > 0);

    if(n > 30) n = 30;

    BartleteCriteria::init();

    int index = -1;


    for(size_t i = 0; i < m_probs.size(); i++)
    {
        if(m_probs[i] == p)
        {
            index = i;
            break;
        }
    }

    assert(index != -1);

    int minInd = 0;
    double min = abs(nVals[0] - n);

    for(size_t i = 0; i < nVals.size(); i++)
    {
        if(min > abs(nVals[i] - n))
        {
            min = abs(nVals[i] - n);
            minInd = i;
        }
    }

    return BartleteCriteria::probTable[minInd * 6 + index];
}

vector<double> BartleteCriteria::SupportedProbs()
{
    init();
    return m_probs;
}
