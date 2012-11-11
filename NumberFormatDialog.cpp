#include "NumberFormatDialog.h"
#include "ui_NumberFormatDialog.h"
#include "NumberFormatter.h"



NumberFormatDialog::NumberFormatDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NumberFormatDialog)
{
    ui->setupUi(this);
}

NumberFormatDialog::~NumberFormatDialog()
{
    delete ui;
}

void NumberFormatDialog::on_okPushButton_clicked()
{
    close();
}

bool NumberFormatDialog::askNewFormat(INumberFormatter* oldFmt)
{
    if(oldFmt)
        switch(oldFmt->format())
        {
        case NumberFormatter::Sci://научный-экспоненциальный
            ui->sci_radioButton_2->setChecked(true);
            break;

        case NumberFormatter::DegMeaning://дробь N ЗНАЧАЩИХ разрядов после запятой
            ui->meaningfulDegCheckBox->setChecked(true);

        case NumberFormatter::Deg://дробь N знаков после запятой
            ui->regular_radioButton_3->setChecked(true);
            ui->degSpinBox->setValue(oldFmt->digits());
            break;
            break;

        default://все оснальное
            ui->auto_radioButton->setChecked(true);
        }

    exec();
    return true;
}

INumberFormatter * NumberFormatDialog::getFormatter()
{
    NumberFormatter::Format f = NumberFormatter::Auto;

    if(ui->regular_radioButton_3->isChecked())
    {
        if(ui->meaningfulDegCheckBox->isChecked())
            f = NumberFormatter::DegMeaning;
        else
            f = NumberFormatter::Deg;
    }
    else if(ui->sci_radioButton_2->isChecked())
        f = NumberFormatter::Sci;

    return new NumberFormatter(f, ui->degSpinBox->value());
}
