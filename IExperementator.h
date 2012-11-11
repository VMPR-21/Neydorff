#ifndef IEXPEREMENTATOR_H
#define IEXPEREMENTATOR_H

#include <vector>
#include "IRegressionCoefficientTable.h"
#include "Events.h"

class IExperimentTable;

struct ExperimentPoint
{
    std::vector<double> xs;
    double yt;
    double yp;
};

//аргумент и возращаемое значение - вектор точек.
//пример:
//ExperimentStepsDelegatePtr delegate;
// RetType retVal=delegate->fire(arg0);
typedef Event< std::vector<ExperimentPoint> , std::vector<ExperimentPoint> >* ExperimentStepsDelegatePtr;

//абстрактный класс для восхождения по градиенту и поиска экстремума.
class IExperementator
{
public:
    //запустить процесс восхождения и поиска экстремума.
    // table - таблица c данными.
    // isMax - true, если ищем максимум. иначе - false

    virtual std::vector<ExperimentPoint> calcStepYt(bool isMax, const IExperimentTable &table, ExperimentStepsDelegatePtr experimentDelegate, bool* isExtremFound) = 0;
    virtual ~IExperementator() {}
};

#endif // IEXPEREMENTATOR_H
