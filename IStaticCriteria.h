#ifndef ISTATICCRITERIA_H
#define ISTATICCRITERIA_H

class IStaticCriteria
{
public:

    virtual bool Check(double value1, int freedom1, double val2, int freedom2) = 0;
};

#endif // ISTATICCRITERIA_H
