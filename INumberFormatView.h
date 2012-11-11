#ifndef INUMBERFORMATVIEW_H
#define INUMBERFORMATVIEW_H

#include "INumberFormatter.h"

//интерфейс для создания пользователем подходящего преобразователя
class INumberFormatView
{
public:
    //спрос. у пользователя
    virtual bool askNewFormat(INumberFormatter* oldFmt = 0) = 0;

    //получить объект
    virtual INumberFormatter* getFormatter() = 0;

    virtual ~INumberFormatView() {}
};

#endif // INUMBERFORMATVIEW_H
