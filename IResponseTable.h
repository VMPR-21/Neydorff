#ifndef IRESPONSETABLE_H
#define IRESPONSETABLE_H

#include <vector>
#include "DataStream.h"
#include "Events.h"
#include "ICriteria.h"
#include <QTextStream>

class ICriteria;
class IExperimentTable;

//receives vector of old(existed) center values. Returns new list.
typedef Event< std::vector<double> , std::vector<double> >* CenterYDelegatePtr;

struct YInfo
{
    double Value;
    bool IsTrusted;
};

// интерфейс таблицы откликов
class IResponseTable
{
public:

    //стат. обработка с использованием указанного критерия и доверительной вероятности
    virtual void statProcessing(ICriteria &criteria, double trust_probability, CenterYDelegatePtr ysRequestDel,bool isFormulaModel) = 0;

    // Методы получения значений откликов
    virtual std::vector<YInfo> at(int row) const = 0;                 // Значения для i-ой строки таблицы
    virtual std::vector<double> trusted_values_at(int row) const = 0; // Не выбросы для i-ой строки таблицы

    // Методы получения У-средх
    virtual double average_at(int row) const = 0;         // У-среднее для i-ой строки, включающее все значения
    virtual double trusted_average_at(int row) const = 0; // У-среднее для i-ой строки, исключая выбросы

    // Методы установки значений откликов
    virtual void set_at(int row, std::vector<YInfo> values) = 0; // Установить значения откликов для i-ой строки
    virtual void add_at(int row, YInfo value) = 0;               // Добавить значение отклика для i-той строки
    virtual void add_at(int row, double value) = 0;

    virtual int count() const = 0;// число строк
    virtual double dispersion(int row) const = 0;

    // проверка однородности дисперсии
    virtual int ongenericalDispersion(StaticCriteria criteria, double prob) = 0;
    virtual double dispersionVospr() const = 0;
    virtual bool checkAdec(const IExperimentTable &table) = 0;

    //загрузка и сохранение в поток данных таблицы
    //для помещения в поток и извлечения из потока использовать операторы >> и <<
    virtual void loadFrom(DataStream &srcStream) = 0;
    virtual void saveTo(DataStream &dstStream) = 0;

    //загрузка и сохранение в CSV
    virtual void loadFromCSV(QTextStream &srcStream) = 0;
    virtual void saveToCSV(QTextStream &dstStream) = 0;
    virtual int getParal() = 0;

    virtual vector<double> trusted_at_center()const = 0;
    virtual double trusted_average_at_center()const = 0;
    virtual double disp_at_center()const = 0;

    virtual const char* getLastError() = 0;
    virtual ~IResponseTable() {}
};

#endif // IRESPONSETABLE_H
