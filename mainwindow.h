#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <IView.h>
#include <QModelIndex>
#include <IUiController.h>
#include <QTableWidgetItem>
#include "Shuffler.h"
#include <QTimer>
#include <QStringList>

namespace Ui
{
class MainWindow;
}


class MainWindow : public QMainWindow , public IView
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //  запускать ли программу - выбор в startmenu
    bool startProgram;



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
private slots:
    void onBtableContextMenu(const QPoint &pt);
    void onClockTimer();

    void onHomogeneityBtnAction(QAction *action);
    void on_pushButton_clicked();

    void on_tableWidget_activated(const QModelIndex &index);

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_ChangeParams_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void saveToFile();//save
    bool loadToFile(); //load
    void saveToCSV();//save to csv
    bool loadFromCSV(); //load from csv
    bool NewExperimenLoadfromCSV(); //
    void chModelParams(); //parametri
    void chNumberFormater();//format chisla
    void nModel(); //new experement
    void nModelFormula();
    void testExperimentTableSlotMax();
    void testExperimentTableSlotMin();
    void testSomeProcessSlot();
    void colaSettingsSlot();

    void on_tableWidget_cellChanged(int row, int column);

    void on_comboBox_activated(const QString &arg1);
    void Combo(int comboInt);
    void Combo2(int comboInt);
    void on_tableWidget_clicked(const QModelIndex &index);

    void on_tableWidget_itemClicked(QTableWidgetItem *item);

    void on_checkBox_MixRows_toggled(bool checked);

    void on_checkBox_normNat_toggled(bool checked);

    void on_homogeneity_Button_clicked();

    void on_gradSearch_Button_clicked();

    void on_calcFunc_btn_clicked();

    void on_pushButton_MAX_clicked();

    void on_pushButton_MIN_clicked();

    void on_pushButton_CCP_clicked();

    void on_findExtrButton_3_clicked();

private:
    //overrided
    void keyPressEvent(QKeyEvent *e);

    Ui::MainWindow *ui;
    IUiController *control;
    QTimer clockTimer;

    QStringList _factorDescriptions;

    bool _dataChanged;
    double _h;
    int _ch;
    int _dev;



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
    void createMenu();  //menu
    void createActions(); //menu
    void createToolBars(); //menu

    void testExperimentTable(bool max, double _h, int _ch, int _dev);

    int stroka;
    int stolbec;
    int xcount;
    int gg, r;
    double diY, diB;
    int _actualInputs, _inputs, _rows;

    bool _showNormed;
    bool _shuffleRows;
    QColor xcolor;
    QColor ycolor;
    QColor bcolor;
    QColor ycolorBad;

    Shuffler *shuffler;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *optionMenu;
    QMenu *helpMenu;
    QMenu *testMenu;
    QAction * testExperimentTableMaxAction;
    QAction * testExperimentTableMinAction;
    QAction * testSomeProcessAction;
    QAction *openActions;
    QAction *saveActions;
    QAction *openFromCSVActions;
    QAction *saveToCSVActions;
    QAction *paramsAction;
    QAction *newActions;
    QAction *newActionsF;
    QAction *colaSettingsAction;

    QAction *formatChisAction;

    QAction *aboutQtAction;
    enum {MaxRecentFiles = 5};
    QAction *recentFileActions[MaxRecentFiles];
    QAction *seporatorAction;
    QToolBar *fileToolBar;

};



#endif // MAINWINDOW_H
