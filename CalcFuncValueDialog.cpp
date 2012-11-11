#include "CalcFuncValueDialog.h"
#include "ui_CalcFuncValueDialog.h"
#include "IExperimentTable.h"
#include "IFactorTable.h"
#include "IRegressionCoefficientTable.h"
#include "bCoeff.h"

CalcFuncValueDialog::CalcFuncValueDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalcFuncValueDialog)
{
    ui->setupUi(this);
    this->table = NULL;
}

CalcFuncValueDialog::~CalcFuncValueDialog()
{
    delete ui;
}

void CalcFuncValueDialog::startCalcDialog(const IExperimentTable &table)
{
    ui->tableWidget->setRowCount(table.x().count());

    for(int i = 0; i < table.x().count(); i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(tr("X%1").arg(i + 1));
        item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
        ui->tableWidget->setItem(i, 0, item);
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem("0.0")); //default val.==0.0
    }

    this->table = &table;

    QString eq;
    const std::vector<bCoeff> &bcs = table.b().coeffs();
    size_t sz = bcs.size();

    for(size_t i = 0; i < sz; i++)
    {
        double v = bcs[i].natural_value();
        QString vstr = QString::number(v);

        if(i && v >= 0)
            vstr = "+" + vstr;

        eq += vstr;
        std::vector<int> index = bcs[i].index(), nc(table.b().count(), 0);

        if(index.size() == 1 && index[0] == 0) // B0 -just as number
            continue;

        for(size_t j = 0; j < index.size(); j++)
        {
            nc[index[j]]++;
        }

        for(size_t j = 0; j < nc.size(); j++)
        {
            if(!nc[j])
                continue;

            eq += tr("*X%1").arg(j);

            if(nc[j] > 1)
                eq += tr("^%1").arg(nc[j]);
        }
    }

    eq += " = 0";
    ui->label_equation->setText(eq);
    updateResult();
    exec();
}

void CalcFuncValueDialog::on_tableWidget_cellChanged(int row, int column)
{
    updateResult();
}

void CalcFuncValueDialog::on_pushButton_clicked()
{
    close();
}

void CalcFuncValueDialog::updateResult()
{
    if(!table)
        return;

    ui->lineEdit->clear();
    std::vector<double> xvals;

    for(int i = 0; i < table->x().count(); i++)
    {
        bool isOk;
        double val = ui->tableWidget->item(i, 1)->text().toDouble(&isOk);

        if(!isOk)
            return;

        xvals.push_back(val);
    }

    ui->lineEdit->setText(QString::number(table->evalFuncValue(xvals)));
}
