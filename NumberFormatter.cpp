#include "NumberFormatter.h"
#include <QString>
#include <stdio.h>
#include <INumberFormatter.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>


void printAsExp(double val, char *out);
void printWithFixedPrecision(double val, int precision, char *out);
void printWithFixedMeaningNumber(double val, int meaning, int delimiter, char *out);

NumberFormatter::NumberFormatter(INumberFormatter::Format fmt, int digs)
{
    currFmt = fmt;
    digitsNum = digs;
}


QString NumberFormatter::toString(double val) const
{
    char sval[54];

    switch(currFmt)
    {
    case Sci://научный-экспоненциальный
        printAsExp(val, sval);
        break;

    case Deg://дробь N знаков после запятой
        printWithFixedPrecision(val, digitsNum, sval);
        break;

    case DegMeaning://дробь N ЗНАЧАЩИХ разрядов после запятой
        printWithFixedMeaningNumber(val, digitsNum, '.', sval);
        break;

    default://все оснальное
        return QString::number(val, 'g', 15);
    }

    return QString(sval);
}


void printAsExp(double val, char *out)
{
    sprintf(out, "%E", val);
}


void printWithFixedPrecision(double val, int precision, char *out)
{
    assert(precision >= 0 && precision <= 15);	// необходимо проверять в View
    char format[8];
    format[0] = '\0';
    strcat(format, "%.");
    sprintf(&format[2], "%d", precision);
    strcat(format, "lf");
    sprintf(out, format, val);
}


int firstindexof(char *str, int ch)
{
    for(int i = 0; str[i] != '\0'; i++)
        if(ch == (int)str[i]) return i;

    return -1;
}


int lastindexof(char *str, int ch)
{
    int index = -1;

    for(int i = 0; str[i] != '\0'; i++)
        if(ch == (int)str[i]) index = i;

    return index;
}


void printWithFixedMeaningNumber(double val, int meaning, int delimiter, char *out)
{
    assert(meaning >= 0 && meaning <= 53);	// необходимо проверять в View
    char sval[54];
    sprintf(sval, "%.15lf", val);	// symbolic representation of val
    int del_index = firstindexof(sval, delimiter);
    int first_meaning_index = -1;

    for(int i = 0; sval[i] != '\0'; i++)
        if(sval[i] > '0' && sval[i] <= '9')  	// 1..9 digit
        {
            first_meaning_index = i;
            break;
        }

    if(first_meaning_index == -1)	// number has no meaning digits
    {
        sprintf(out, "%.0lf", val);
    }

    int digit_count = first_meaning_index;

    for(int i = digit_count, mean_found = 0; sval[i] != '\0'; i++)
    {
        if(sval[i] > '0' && sval[i] <= '9')	// 1..9
            mean_found++;

        if(mean_found == meaning)
        {
            digit_count = i;
            break;
        }
    }

    if(digit_count < del_index)
    {
        sprintf(out, "%.0lf", val);	// if digit_count less than meaning digit count in integer part of double
        return;
    }

    if(sval[digit_count + 1] == '\0' || sval[digit_count + 1] < '5')
    {
        for(int i = digit_count; i > first_meaning_index; i--)
            if(sval[i] == '0') --digit_count;
            else break;
    }

    printWithFixedPrecision(val, digit_count - del_index, out);
}

INumberFormatter::Format NumberFormatter::format() const
{
    return currFmt;
}

int NumberFormatter::digits() const
{
    return digitsNum;
}
