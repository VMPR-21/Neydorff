#ifndef NUMBERFORMATDIALOG_H
#define NUMBERFORMATDIALOG_H

#include <QDialog>
#include "INumberFormatView.h"

namespace Ui
{
class NumberFormatDialog;
}

class INumberFormatter;

//Реализация окна выбора параметров форматирования чисел.
class NumberFormatDialog : public QDialog, public INumberFormatView
{
    Q_OBJECT

public:
    explicit NumberFormatDialog(QWidget *parent = 0);
    virtual ~NumberFormatDialog();

    //Реализация INumberFormatView
    bool askNewFormat(INumberFormatter* oldFmt = 0);
    INumberFormatter* getFormatter();

private slots:
    void on_okPushButton_clicked();

private:
    Ui::NumberFormatDialog *ui;
};

#endif // NUMBERFORMATDIALOG_H
