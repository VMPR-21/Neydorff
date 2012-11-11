#ifndef OPTIMUMSEARCHSETTINGSDIALOG_H
#define OPTIMUMSEARCHSETTINGSDIALOG_H

#include <QDialog>
#include <vector>
#include "IExperementator.h"

namespace Ui
{
class OptimumSearchSettingsDialog;
}

class IExperimentTable;
class OptimumSearchSettingsDialog : public QDialog
{
    Q_OBJECT

    bool _isMax;
public:
    explicit OptimumSearchSettingsDialog(QWidget *parent = 0);
    ~OptimumSearchSettingsDialog();

    std::vector<double> askAndSearch(const IExperimentTable &table);
    std::vector<double> askAndSearch(const IExperimentTable &table, ExperimentPoint vals);
private slots:
    void on_pushButtonMax_clicked();

    void on_pushButtonMin_clicked();

private:
    Ui::OptimumSearchSettingsDialog *ui;
};

#endif // OPTIMUMSEARCHSETTINGSDIALOG_H
