#ifndef EXPERIMENTDIALOG_H
#define EXPERIMENTDIALOG_H

#include <QDialog>
#include "IExperimentDialog.h"
#include "IExperimentTable.h"
#include <IView.h>
#include <QModelIndex>
#include <IUiController.h>
#include <QTableWidgetItem>
#include "Shuffler.h"
#include <QStringList>
#include "IExperementator.h"

class INumberFormatView;
class INumberFormatter;
class IResponcesSource;
class IExperimentTable;

namespace Ui
{
class ExperimentDialog;
}

class ExperimentDialog : public QDialog, public IExperimentDialog, public IView
{
    Q_OBJECT

public:

    IExperimentTable *_experimentTable;
    explicit ExperimentDialog(QWidget *parent = 0);
    virtual ~ExperimentDialog();
    void startDialog(const QString &windowTitle, IExperimentTable *table, IExperimentTable *pfe_table, int iteraction_level);
    //    private:
    //        Ui::ExperimentDialog *ui;
    //данные бедется из table и туда же помещаются. Там же есть методы для запуска расчетов.

    //сообщает интерфейсу о необходимости обновить на экране значения "Х" из таблицы
    void updateInputs(const IExperimentTable &table);

    ////сообщает интерфейсу о необходимости обновить значения "У" из таблицы
    void updateYY(const IExperimentTable &table);

    //сообщает интерфейсу о необходимости обновить коэф-ты ур-ия
    void updateBcoefs(const IExperimentTable &table);

    //сообщает интерфейсу о необходимости обновить значения Градиента
    void updateGr(const IExperimentTable &table, const std::vector<ExperimentPoint> &points);

    //заполнение таблицы данными(Х,У...)
    void readData(IExperimentTable *table);

    void updateAdeq(const IExperimentTable &table);


    //Реализация IUiController
    void newModel(bool createEmpty = false);

    bool loadModel(const QString &fileName);

    bool saveModel(const QString &fileName);

    void calcY(double threshold);

    void calcB(double threshold);

    INumberFormatter* getNumberFormatter();

    void changeNumberFormater();

    void changeFactorParams(int factorIndex);

    void calcExtremum(bool isMax);

    void calcAdeq();

    int checkHomogeneity(StaticCriteria criteria, const char **strErr);

    std::vector<double> gradientSearch();

    void calcFuncValue();

    void changeModelParams();

    void dataChanged();

    void needRefresh(bool inputsOnly = true);

    void findExtr();

private slots:
    void onBtableContextMenu(const QPoint &pt);
    void on_tableWidget_activated(const QModelIndex &index);

    void on_pushButton_11_clicked();

    void on_pushButton_2_clicked();

    void chModelParams(); //parametri
    void chNumberFormater();//format chisla
    void nModel(); //new experement

    void on_tableWidget_cellChanged(int row, int column);

    void on_comboBox_activated(const QString &arg1);
    void Combo(int comboInt);
    void Combo2(int comboInt);
    void on_tableWidget_clicked(const QModelIndex &index);
    /////////
    void on_tableWidget_itemClicked(QTableWidgetItem *item);
    /////////////
    void on_checkBox_MixRows_toggled(bool checked);
    //////////
    void on_checkBox_normNat_toggled(bool checked);


    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

private:
    bool _modelHasData;
    IResponcesSource *_dataSrc;
    INumberFormatView *_formatterView;
    INumberFormatter *_formatter;
    IView *_view;
    int _interaction_level;

    std::vector<double> centerYrequester(const std::vector<double> &centerVals);
    std::vector<ExperimentPoint> experimentalResponcesRequester(const std::vector<ExperimentPoint> &centerVals);
    std::vector<double> getProbsList(StaticCriteria criteria);
    void initEmptyModel();
    void setDataSource(int baseFactors, int factorsCount, const std::vector<double> &centers,
                       const std::vector<double> &deltas, QStringList descriptions);


    Ui::ExperimentDialog *ui;
    //overrided
    void keyPressEvent(QKeyEvent *e);

    IUiController *control;
    IExperimentTable * _table;
    QStringList _factorDescriptions;

    bool _dataChanged;

    void cellpaint(int a, int b, const QColor &color);
    void cellpaintB(int a, int b, const QColor &color);
    void cellpaintY(int a, int b, const QColor &color);

    //shuffite
    void checkShuffler(int rows);
    int getGuiIdx(int idx);
    int getRealIdx(int guiRow);

    int countColsCount(const IExperimentTable &table);
    void showYdata(const IExperimentTable &table);

    void updateHader(const QStringList &factorDescriptions);
    void updateHader1();
    void updateHader2();

    int stroka;
    int stolbec;
    int xcount;
    int gg, r;
    double diY, diB;
    int _actualInputs, _inputs, _rows;
    void changeBcoefSignificance(int row);
    bool _showNormed;
    bool _shuffleRows;
    QColor xcolor;
    QColor ycolor;
    QColor bcolor;
    QColor ycolorBad;

    Shuffler *shuffler;

};

#endif // EXPERIMENTDIALOG_H
