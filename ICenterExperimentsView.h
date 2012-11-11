#ifndef ICENTEREXPERIMENTSVIEW_H
#define ICENTEREXPERIMENTSVIEW_H

#include <vector>

//Интерфейс для окна ввода центров плана.
class ICenterExperimentsView
{
public:

    // запрос у пользователя значений центра плана. Аргумент yy - текущие значения в центре плана.
    virtual bool askCenterYs(const std::vector<double> &yy) = 0;

    //получить значения центра плана
    virtual std::vector<double> getCenterYs() const = 0;

    virtual ~ICenterExperimentsView() {}
};

#endif // ICENTEREXPERIMENTSVIEW_H
