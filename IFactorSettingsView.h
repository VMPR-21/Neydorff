#ifndef IFACTORSETTINGSVIEW_H
#define IFACTORSETTINGSVIEW_H

#include <vector>

class IFactorTable;

//Интерфейс для окна выбора генерирующего соотношения для фактора
class IFactorSettingsView
{
public:

    //запрос у пользователя комбинации из базовых вакторов
    //и oldSet как предыдущей комбинации
    virtual bool askFactorSettings(int factorIndex, const IFactorTable *table) = 0;

    //получение комбинации факторов
    virtual std::vector<int> getSetting() = 0;

    virtual ~IFactorSettingsView() {}
};

#endif // IFACTORSETTINGSVIEW_H
