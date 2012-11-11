#ifndef NUMBERFORMATTER_H
#define NUMBERFORMATTER_H

#include "INumberFormatter.h"

class QString;

//Объект-форматтер для чисел. Позволяет получить строкове представления
//числа типа double в заданом формате Format
class NumberFormatter : public INumberFormatter
{
    Format currFmt;
    int digitsNum;

public:
    NumberFormatter(Format fmt, int digs);

    virtual QString toString(double val) const;
    virtual Format format() const;
    virtual int digits() const;
};

#endif // NUMBERFORMATTER_H
