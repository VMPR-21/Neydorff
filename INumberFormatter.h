#ifndef INUMBERFORMATTER_H
#define INUMBERFORMATTER_H
#include <QString>

//преобразование числа в строку определённого формата
class INumberFormatter
{
public:
    enum Format
    {
        Auto,//Автоматический выбор представления числа
        Sci,//Экспоненциалная форма
        Deg,//дробь с опред-ым числом знаков после запятой
        DegMeaning//дробь с опред-ым числом Значимых разрядов после запятой
    };

    //преобразование числа в строку согласно текущим настройкам
    virtual QString toString(double val) const = 0;

    //получение текущих настроек форматирования
    virtual Format format() const = 0;
    virtual int digits() const = 0;

    virtual ~INumberFormatter() {}
};

#endif // INUMBERFORMATTER_H
