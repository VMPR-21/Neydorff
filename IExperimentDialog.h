#ifndef IEXPERIMENTDIALOG_H
#define IEXPERIMENTDIALOG_H

#endif // IEXPERIMENTDIALOG_H

#include "IExperimentTable.h"

class IExperimentDialog
{
public:
    virtual void startDialog(const QString &windowTitle, IExperimentTable *table, IExperimentTable *pfe_table, int interaction_level) = 0;
    //данные бедется из table и туда же помещаются. Там же есть методы для запуска расчетов.

    virtual ~IExperimentDialog() {}
};
