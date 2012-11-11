
#include "vector"

using namespace std;

class FisherCriteria
{
    static bool initialized;
    static vector<double> fisherTable;
    static void _init();
    static void init()
    {
        if(!FisherCriteria::initialized)
            FisherCriteria::_init();
    }
public:
    FisherCriteria();
    static double GetFisherVal(int minF, int maxF);
    static vector<double> SupportedProbs();
};

