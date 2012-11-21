
#ifndef NewExperimentSettingsDialog_H
#define NewExperimentSettingsDialog_H

// Factor tableview params
#define FACT_NAME_COLUMN 1
#define FACT_CENTER_COLUMN 2
#define FACT_DIAPAZON_COLUMN 3
#define FACT_MIN_COLUMN 4
#define FACT_MAX_COLUMN 5
#define FACT_INTERVAL_COLUMN 6


#include <QDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QKeyEvent>
#include <QClipboard>
#include <QtAlgorithms>
#include <math.h>
#include <QFileDialog>
#include <QSettings>

#include "IModelSettingsView.h"
#include "ResponcesSource.h"
#include "ClipCopyPaste.h"
#include "ResponcesSourseFunction.h"

# include "NewExperimentSettingsDialog.h"
# include "ui_NewExperimentSettingsDialog.h"

#include "formula_setting_help_dialog.h"

#define MATHMODEL 1
#define OPTIMASEARCH 2

namespace Ui
{
class NewExperimentSettingsDialog;
}

class NewExperimentSettingsDialog : public QDialog, public IModelSettingsView
{
    Q_OBJECT

public:
    explicit NewExperimentSettingsDialog(short ModelType = MATHMODEL, IResponcesSource *pDataSource = NULL, int _interaction_level = 0, int _paral = 0, QWidget *parent=0);
    virtual ~NewExperimentSettingsDialog();

    //получение источника данных
    IResponcesSource* getDataSource();
    //запрос настроек у пользователя
    bool askNewSettings();
    int getInteractionLevel();
    int GetModelType();
    int getParal();

//   / int closeEvent(QCloseEvent *evnt);
    //void closeEvent(QCloseEvent *);



private slots:
    void on_FactNum_spinBox_valueChanged(int arg1);
    void on_DrobRepl_horizontalSlider_valueChanged(int value);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_tableWidget_Znach_cellChanged(int row, int column);

    void on_spinBox_paral_valueChanged(int arg1);

    void on_tFormulaInput_textChanged();

    void on_btnShowHelp_clicked();

    void on_rbtnGetYFromFunction_toggled(bool checked);

    void on_rbtnGetYFromNatur_toggled(bool checked);


    void on_saveFormula_clicked();

    void on_loadFormula_clicked();

private:
    Ui::NewExperimentSettingsDialog *ui;

    int _interaction_level;
    short _modelType;

    int _paral;
    //ResponcesSourse data;
    //ResponcesSource data_old;
    ResponcesSourseFunction data;
    ResponcesSourseFunction data_old;
    QString EvaluateString;
    QVector<QString> FactorNames;

    struct FactInfo
    {
        QString FactorName; //1
        double Center; //2
        double Interval; //3
    };

    QVector<FactInfo> AllPrevFactors;

    bool FactorExists(QVector<QString> FactorList,QString FactorName);
    void updateTables();
    void updateTables(QString factorname);
    void calcRealFactNum();
    void calcMinFactNum();
    bool saveFactValues();
    void init();
    bool checkValues();
    void onIntervalChanged(int row, int col);
    void onCenterChanged(int row, int col);
    void onMinMaxChanged(int row, int col);
    void loadSettings();
    void initValues();
    void keyPressEvent(QKeyEvent *);
    void clipCopy(QTableWidget *table);
    void clipPaste(QTableWidget *table);

    QString CheckExpressionPattern(QString pattern,QString text);
    QString ColorText(QString text, QString htmlFont);
    QVector<QString> GetExpressionFactorNames(QString text);
    QString ColorFormula(QString text);


    void FormulaCheck();

    FactInfo GetFactorInfo(QString FactorName);
    void     SetFactorInfo(QString FactorName, double Center, double Interval);
    bool SortVaribles(QString &s1,QString &s2);

    FactInfo GetFactorInfo(int FactorNumber);
    void     SetFactorInfo(int FuctorNumber,QString FactorName, double Center, double Interval);



    bool isFormulaReferense;
    bool haswronWaribles;
    bool allowPsevdonamesForVaribles;
    int wrongpos;
    bool canceled;
    bool def;
    bool auto_check;
    bool savedata;

};

#endif // NewExperimentSettingsDialog_H


