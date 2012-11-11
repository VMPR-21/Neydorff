#ifndef IMAGINARYSTEPS_H
#define IMAGINARYSTEPS_H

#include <QDialog>
#include "IExperementator.h"

namespace Ui
{
class ImaginarySteps;
}

class ImaginarySteps : public QDialog
{
    Q_OBJECT


public:
    explicit ImaginarySteps(const std::vector<int> &FactorsNumbers, const std::vector<ExperimentPoint> &points , QWidget *parent = 0);
    ~ImaginarySteps();
    std::vector<ExperimentPoint> getYp();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ImaginarySteps *ui;
    std::vector<ExperimentPoint> resultPoints;
    int numberOfFactors;
};

#endif // IMAGINARYSTEPS_H
