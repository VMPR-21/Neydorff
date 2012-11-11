#ifndef IUICONTROLLER_H
#define IUICONTROLLER_H

#include <QString>
#include "ICriteria.h"
#include "IExperimentTable.h"

class INumberFormatter;

//Абстрактный класс контроллера, которому отправляются команды от интерфейса пользователя(IView)
class IUiController
{
public:

    //создать новую модель
    virtual bool newModel(bool createEmpty = false, bool isFormulaModel = false) = 0;
    //virtual void newModel(bool createEmpty = false, bool isFormulaModel = false) = 0;

    //создать новую модель
    virtual void newModellGrad(bool _isFormulaModel2) = 0;

    //загрузить из файла
    virtual bool loadModel(const QString &fileName) = 0;

    //сохранить в файл
    virtual bool saveModel(const QString &fileName) = 0;
    //virtual bool setDataSourceType(enum DataSourceType dataSourceType)=0;

    virtual bool NewExperimenLoadfromCSV(const QString &fileName) = 0;

    //расчитать "У" с указаным порогового значения для отбрасывания выбросов
    virtual void calcY(double threshold) = 0;

    //расчитать "B"
    virtual void calcB(double threshold) = 0;

//
    //получить объект для форматирования чисел double по необходимым параметрам
    virtual INumberFormatter* getNumberFormatter() = 0;

    //запустить изменение параметров форматирования чисел
    virtual void changeNumberFormater() = 0;

//
    //запустить изменение параметров модели. Новые параметры запрашиваются у пользователя.
    virtual bool changeModelParams() = 0;
    //virtual void changeModelParams() = 0;

//

    //запустить изменение параметров для указаного фактора("составного") с индексом factorIndex.
    //индекс имеет диапазон: [0; inputs-actualInputs]
    virtual void changeFactorParams(int factorIndex) = 0;

//
    //расчитать точки экстремума
    // isMax - true, если ищем максимум. иначе - false
    virtual void calcExtremum(bool isMax) = 0;

    //Проверить адекватность уравнения
    virtual void calcAdeq() = 0;

    //Выполнить градиентный поиск
    virtual std::vector<double> gradientSearch() = 0;

    //посчитать значение полученной функции (уравнения регрессии) для какой-либо точки пространства
    virtual void calcFuncValue() = 0;

    //Проверка однородности
    virtual int checkHomogeneity(StaticCriteria criteria, const char **strErr) = 0;

    //вызывается видом при завершении изменений в данных, когда необходимо учесть пользовательский ввод
    // например перед началом расчетов
    virtual void dataChanged() = 0;

    //изменить значимость коэфф-та "В".
    virtual void changeBcoefSignificance(int row) = 0;

    //вызывается видом при необходимости обновить значения на экране
    virtual void needRefresh(bool inputsOnly = true) = 0;
    virtual ~IUiController() {}

    virtual IExperimentTable * getExperimentTable() = 0;
    virtual void startProcess(bool max, double _h, int _ch, int _dev) = 0;
    virtual void CCP() = 0;

    virtual void findExtr() = 0;
};

#endif // IUICONTROLLER_H
