#ifndef FINDEXTR_H
#define FINDEXTR_H

#include <QDialog>
#include "IExperimentTable.h"
#include "IFactorTable.h"
#include "IRegressionCoefficientTable.h"
#include "bCoeff.h"
#include "Experementator.h"
#include "QModelIndex"
#include "ResponcesSourseFunction.h"

namespace Ui {
class findExtrDialog;
}


class IExperimentTable;
class findExtrDialog : public QDialog
{
    Q_OBJECT
    
    const IExperimentTable *table;
    IResponcesSource *src;

    void updateResult();



    bool initial;

    std::vector<double> grad;

    double xP;  //шаг, начиная с какого перевалило за 1
    bool xP_find;
    double yt;
    ExperimentPoint P;


    void findExtr();
    void restartfindExtr();
    void restartfindExtr_1();
    void updateTable();
    bool calcYpp();

public:
    explicit findExtrDialog(QWidget *parent = 0);
    ~findExtrDialog();
    void startfindExtr(IExperimentTable &table, IResponcesSource &_src);

    std::vector<ExperimentPoint> v;
    bool isExtremFound;

    bool ccp;
    bool exper;

    int index1; //точка в которой провести эксперимент
    int index2; //точка в которой провести эксперимент

    bool isMax;
    double h;
    int ch;
    double dev;

    bool isFormulaExperiment;

    double _FactDivergences;

    void saveToCSV(const QString &fileName);


private slots:
    void on_tableWidget_2_cellChanged();

    void on_pushButton_clicked();

    void on_pushButton_CCP_clicked();

    void on_pushButton_Experiment_clicked();

    void on_doubleSpinBox_editingFinished();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_tableWidget_2_doubleClicked(const QModelIndex &index);

    void on_pushButton_2_clicked();

private:
    Ui::findExtrDialog *ui;

    bool h_edit;

    std::vector<QString> v_yp;


};

#endif // FINDEXTR_H
