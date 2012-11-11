#ifndef CALCFUNCVALUEDIALOG_H
#define CALCFUNCVALUEDIALOG_H

#include <QDialog>

namespace Ui
{
class CalcFuncValueDialog;
}

class IExperimentTable;
class CalcFuncValueDialog : public QDialog
{
    Q_OBJECT

    const IExperimentTable *table;

    void updateResult();
public:
    explicit CalcFuncValueDialog(QWidget *parent = 0);
    ~CalcFuncValueDialog();

    void startCalcDialog(const IExperimentTable &table);

private slots:
    void on_tableWidget_cellChanged(int row, int column);

    void on_pushButton_clicked();

private:
    Ui::CalcFuncValueDialog *ui;
};

#endif // CALCFUNCVALUEDIALOG_H
