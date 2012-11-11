#ifndef FACTORSETTINGSDIALOG_H
#define FACTORSETTINGSDIALOG_H

#include <QDialog>
#include "IFactorSettingsView.h"
#include <QListWidgetItem>

namespace Ui
{
class FactorSettingsDialog;
}

//Интерфейс для окна выбора генерирующего соотношения для фактора
class FactorSettingsDialog : public QDialog, public IFactorSettingsView
{
    Q_OBJECT

public:
    explicit FactorSettingsDialog(QWidget *parent = 0);
    ~FactorSettingsDialog();

    //реализация IFactorSettingsView:

    bool askFactorSettings(int factorIndex, const IFactorTable *table);
    std::vector<int> getSetting();

private slots:

    void on_listWidget_itemSelectionChanged();

    void on_pushButtonOK_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::FactorSettingsDialog *ui;
    std::vector<int> res;

    int _factorIndex;
    const IFactorTable *table;
    bool invalidState;

    void updateResult();
};

#endif // FACTORSETTINGSDIALOG_H
