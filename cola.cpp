#include "cola.h"
#include "ui_cola.h"

Cola::Cola(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Cola)
{
    ui->setupUi(this);
}

Cola::~Cola()
{
    delete ui;
}

void Cola::getParams(double &a, int &b, int &c)
{
    ui->doubleSpinBox->setValue(a);
    ui->spinBox->setValue(b);
    ui->spinBox_2->setValue(c);
    this->exec();
    a = ui->doubleSpinBox->value();
    b = ui->spinBox->value();
    c = ui->spinBox_2->value();
}

void Cola::on_doubleSpinBox_valueChanged(double arg1)
{

}

void Cola::on_pushButton_clicked()
{
    close();
}
