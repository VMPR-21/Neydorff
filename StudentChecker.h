#ifndef STUDENTCHECKER_H
#define STUDENTCHECKER_H
#include "ICriteria.h"

using namespace std;

class StudentChecker : public ICriteria
{
private:
    double Average(vector<double> &mas) const;
    int SuspiciousValue(vector<double> &mas, double average) const;

public:
    StudentChecker();
    vector<double> CheckValues(vector<double> values, double prob) const;
    const char* name() const;
};
#endif // STUDENTCHECKER_H
