#ifndef FORMULA_SETTING_HELP_DIALOG_H
#define FORMULA_SETTING_HELP_DIALOG_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
namespace Ui {
class formula_setting_help_dialog;
}

class formula_setting_help_dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit formula_setting_help_dialog(QWidget *parent = 0);
    ~formula_setting_help_dialog();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::formula_setting_help_dialog *ui;
};

#endif // FORMULA_SETTING_HELP_DIALOG_H
