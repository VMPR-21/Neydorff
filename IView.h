#ifndef IVIEW_H
#define IVIEW_H

#include "IExperementator.h"

class IExperimentTable;

//абстрактный класс для интерфейса пользователя
class IView
{
public:

    //сообщает интерфейсу о необходимости обновить на экране значения "Х" из таблицы
    virtual void updateInputs(const IExperimentTable &table) = 0;

    ////сообщает интерфейсу о необходимости обновить значения "У" из таблицы
    virtual void updateYY(const IExperimentTable &table) = 0;

    //сообщает интерфейсу о необходимости обновить коэф-ты ур-ия
    virtual void updateBcoefs(const IExperimentTable &table) = 0;

    //сообщает интерфейсу о необходимости обновить значения Градиента
    virtual void updateGr(const IExperimentTable &table, const std::vector<ExperimentPoint> &points) = 0;

    virtual void updateAdeq(const IExperimentTable &table) = 0;

    //заполнение таблицы данными(Х,У...)
    virtual void readData(IExperimentTable *table) = 0;

    virtual ~IView() {}
};

#endif // IVIEW_H
