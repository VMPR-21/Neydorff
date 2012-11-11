#ifndef ICRITERIA_H
#define ICRITERIA_H
#include "vector"

using namespace std;

enum StaticCriteria
{
    Fisher,
    Bartlet,
    Kohren
};

class ICriteria
{
public:
    virtual vector<double> CheckValues(vector<double> values, double prob) const = 0;
    virtual const char* name() const = 0;

    virtual ~ICriteria() {}
};

#endif // ICRITERIA_H
