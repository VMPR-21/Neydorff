#ifndef EXPEREMENTATOR_H
#define EXPEREMENTATOR_H


#include "IExperementator.h"
#include <vector>
#include "IRegressionCoefficientTable.h"


class IExperimentTable;

//абстрактный класс для восхождения по градиенту и поиска экстремума.
class Experementator : public IExperementator
{
public:

    //запустить процесс восхождения и поиска экстремума.
    // table - таблица c данными.
    // isMax - true, если ищем максимум. иначе - false
    // isExtremFound - найден экстремум или нет, последняя точка или экстремум или точка в которой проводи эксперимент
    virtual std::vector<ExperimentPoint> calcStepYt(bool isMax, const IExperimentTable &table, ExperimentStepsDelegatePtr experimentDelegate, bool* isExtremFound);


    //Величина начального шага
    double h;

    //количесвто шагов
    unsigned int ch;

    //допустимое отклонение
    double dev;

    //Величина начального шага - h, количесвто шагов - ch, допустимое отклонение - dev
    Experementator(double h = 0.5, unsigned int ch = 7, double dev = 30);

};

#endif // EXPEREMENTATOR_H
