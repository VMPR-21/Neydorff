#ifndef ICOPPOINTSVIEW_H
#define ICOPPOINTSVIEW_H

#include <QString>
#include "IExperimentTable.h"

// COP(Central Orthogonal Plan) - ЦКП.
//
class IExperimentPointsView
{
public:
    virtual void start(const QString &title, IExperimentTable *table) = 0;
    virtual ~IExperimentPointsView() {}
};

#endif // ICOPPOINTSVIEW_H
