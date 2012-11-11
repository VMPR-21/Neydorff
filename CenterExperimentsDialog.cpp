#include "CenterExperimentsDialog.h"
#include "ui_CenterExperimentsDialog.h"

CenterExperimentsDialog::CenterExperimentsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CenterExperimentsDialog)
{
    ui->setupUi(this);
}

CenterExperimentsDialog::~CenterExperimentsDialog()
{
    delete ui;
}

bool CenterExperimentsDialog::askCenterYs(const std::vector<double> &yy)
{
    for(uint i = 0; i < yy.size(); i++)
    {
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(yy[i])));
    }

    exec();
    return true;
}

std::vector<double> CenterExperimentsDialog::getCenterYs() const
{
    std::vector<double> res;

    for(int i = 0; i < ui->tableWidget->rowCount() - 1; i++)
    {
        bool ok;
        double r = ui->tableWidget->item(i, 0)->text().toDouble(&ok);

        if(ok)
            res.push_back(r);
    }

    return res;
}

void CenterExperimentsDialog::on_pushButtonOK_clicked()
{
    close();
}

void CenterExperimentsDialog::on_tableWidget_cellClicked(int row, int column)
{
    if(row + 1 == ui->tableWidget->rowCount())
    {
        //add new
        addNew();
    }
}

void CenterExperimentsDialog::on_pushButton_Add_clicked()
{
    addNew();
}

void CenterExperimentsDialog::on_pushButton_Del_clicked()
{
    int row = ui->tableWidget->currentRow();

    if(row + 1 == ui->tableWidget->rowCount())
        return;

    ui->tableWidget->removeRow(row);
}

void CenterExperimentsDialog::addNew()
{
    int row = ui->tableWidget->rowCount() - 1;
    ui->tableWidget->insertRow(row);
    ui->tableWidget->setItem(row, 0, new QTableWidgetItem("0.0"));
}
