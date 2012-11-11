#ifndef SettingsDialog_ver2_H
#define SettingsDialog_ver2_H

#include <QDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QClipboard>
#include "IModelSettingsView.h"
#include "ResponcesSource.h"
#include "ui_SettingsDialog_ver2.h"
#include <math.h>
#include "ClipCopyPaste.h"

namespace Ui
{
class SettingsDialog_ver2;
}

class SettingsDialog_ver2 : public QDialog, public IModelSettingsView
{
    Q_OBJECT

public:
    explicit SettingsDialog_ver2(IResponcesSource* pDataSource = NULL, int _interaction_level = 1, QWidget *parent = 0);
    virtual ~SettingsDialog_ver2();

    //получение источника данных
    IResponcesSource* getDataSource();
    //запрос настроек у пользователя
    bool askNewSettings();
    int getInteractionLevel();

private slots:
    void on_FactNum_spinBox_valueChanged(int arg1);
    void on_DrobRepl_horizontalSlider_valueChanged(int value);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_tableWidget_Znach_cellChanged(int row, int column);

    void on_spinBox_paral_valueChanged(int arg1);

private:
    Ui::SettingsDialog_ver2 *ui;

    int _interaction_level;

    ResponcesSource data;
    ResponcesSource data_old;

    void updateTables();
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

    bool canceled;
    bool def;
    bool auto_check;
};

#endif // SettingsDialog_ver2_H
