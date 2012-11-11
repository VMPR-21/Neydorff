#ifndef CENTEREXPERIMENTSDIALOG_H
#define CENTEREXPERIMENTSDIALOG_H

#include <QDialog>
#include "ICenterExperimentsView.h"
#include <QModelIndex>

namespace Ui
{
class CenterExperimentsDialog;
}

//Реализация окна ввода значений центра плана
class CenterExperimentsDialog : public QDialog, public ICenterExperimentsView
{
    Q_OBJECT

public:
    explicit CenterExperimentsDialog(QWidget *parent = 0);
    ~CenterExperimentsDialog();

    //Реализация ICenterExperimentsView
    bool askCenterYs(const std::vector<double> &yy);
    std::vector<double> getCenterYs() const;


private slots:
    void on_pushButtonOK_clicked();

    void on_tableWidget_cellClicked(int row, int column);

    void on_pushButton_Add_clicked();

    void on_pushButton_Del_clicked();

private:
    void addNew();
    Ui::CenterExperimentsDialog *ui;
};

#endif // CENTEREXPERIMENTSDIALOG_H
