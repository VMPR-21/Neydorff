#include "formula_setting_help_dialog.h"
#include "ui_formula_setting_help_dialog.h"

formula_setting_help_dialog::formula_setting_help_dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::formula_setting_help_dialog)
{
    ui->setupUi(this);

    QString helptext="";

    QFile file(":/formula_settings_dialog_help_info.txt");
    if (!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        helptext="Невозможно открыть файл справки";
    }
    else
    {
        QTextStream ts(&file);

        while (!ts.atEnd())
        {
            helptext+=QString::fromLocal8Bit(ts.readLine().toUtf8());
        }
    }
    ui->textBrowser->setHtml(helptext);
}

formula_setting_help_dialog::~formula_setting_help_dialog()
{
    delete ui;
}

void formula_setting_help_dialog::on_pushButton_clicked()
{
    close();
}
