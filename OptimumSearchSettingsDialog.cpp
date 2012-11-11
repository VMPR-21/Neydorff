#include "OptimumSearchSettingsDialog.h"
#include "ui_OptimumSearchSettingsDialog.h"
#include "OptimumSearch.h"
#include "IExperimentTable.h"
#include "IFactorTable.h"

OptimumSearchSettingsDialog::OptimumSearchSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptimumSearchSettingsDialog)
{
    ui->setupUi(this);
}

OptimumSearchSettingsDialog::~OptimumSearchSettingsDialog()
{
    delete ui;
}

std::vector<double> OptimumSearchSettingsDialog::askAndSearch(const IExperimentTable &table)
{
    //x
    ui->tableWidget->setRowCount(table.x().count());

    for(int i = 0; i < table.x().count(); i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(tr("X%1").arg(i + 1));
        item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
        ui->tableWidget->setItem(i, 0, item);
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem("0.0")); //default val.==0.0
    }

    exec();
    std::vector<double> x0;

    //make x0
    for(int i = 0; i < table.x().count(); i++)
        x0.push_back(ui->tableWidget->item(i, 1)->text().toDouble());

    return OptimumSearch::FindByGradientMethod(
               table, x0, ui->eps->value(), ui->eps1->value(),
               /*ui->grad_length->value(),ui->dela_length->value(),*/
               ui->lambda->value(), ui->beta->value(), _isMax);
}

std::vector<double> OptimumSearchSettingsDialog::askAndSearch(const IExperimentTable &table, ExperimentPoint vals)
{
    //x
    ui->tableWidget->setRowCount(table.x().count());

    for(int i = 0; i < table.x().count(); i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(tr("X%1").arg(i + 1));
        item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
        ui->tableWidget->setItem(i, 0, item);
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(vals.xs.at(i)))); //default val.==0.0
    }

    exec();
    std::vector<double> x0;

    //make x0
    for(int i = 0; i < table.x().count(); i++)
        x0.push_back(ui->tableWidget->item(i, 1)->text().toDouble());

    return OptimumSearch::FindByGradientMethod(
               table, x0, ui->eps->value(), ui->eps1->value(),
               /*ui->grad_length->value(),ui->dela_length->value(),*/
               ui->lambda->value(), ui->beta->value(), _isMax);
}

void OptimumSearchSettingsDialog::on_pushButtonMax_clicked()
{
    _isMax = true;
    close();
}

void OptimumSearchSettingsDialog::on_pushButtonMin_clicked()
{
    _isMax = false;
    close();
}
