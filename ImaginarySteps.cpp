#include "ImaginarySteps.h"
#include "ui_ImaginarySteps.h"
#include "QMessageBox"

ImaginarySteps::ImaginarySteps(const std::vector<int> &FactorsNumbers, const std::vector<ExperimentPoint> &points , QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImaginarySteps)
{
    ui->setupUi(this);


    this->setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowMinMaxButtonsHint);
    QStringList labels;

    for(size_t i = 0; i < FactorsNumbers.size(); i++)
        labels.append("x" + QString::number(FactorsNumbers.at(i)));

    labels.append("Y теоретическое");
    labels.append("Y практическое");
    ui->tableWidget->setRowCount(points.size());
    ui->tableWidget->setColumnCount(FactorsNumbers.size() + 2);
    ui->tableWidget->setHorizontalHeaderLabels(labels);
    numberOfFactors = FactorsNumbers.size();

    for(size_t i = 0; i < points.size(); i++)
    {
        ExperimentPoint p = points.at(i);

        for(size_t j = 0; j < p.xs.size(); j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(p.xs.at(j)));
            item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable); //нередактируемость
            ui->tableWidget->setItem(i, j, item);
        }

        QTableWidgetItem *item = new QTableWidgetItem(QString::number(p.yt));
        item->setFlags(item->flags() | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        ui->tableWidget->setItem(i, FactorsNumbers.size(), item);
        item = new QTableWidgetItem();
        ui->tableWidget->setItem(i, FactorsNumbers.size() + 1, item);
    }

    resultPoints = points;
    ui->tableWidget->resizeColumnsToContents();

}

ImaginarySteps::~ImaginarySteps()
{
    delete ui;
}

std::vector<ExperimentPoint> ImaginarySteps::getYp()
{
    this->exec();

    return resultPoints;
}

void ImaginarySteps::on_buttonBox_accepted()
{
    bool ok = true;

    for(int i = 0; i < ui->tableWidget->rowCount(); i++)
        if(ui->tableWidget->item(i, numberOfFactors + 1)->text().isEmpty())
            ok = false;

    if(ok)
    {
        for(int i = 0; i < ui->tableWidget->rowCount(); i++)
            resultPoints[i].yp = ui->tableWidget->item(i, numberOfFactors + 1)->text().toDouble();

        this->accept();
    }
    else
        QMessageBox::warning(this, "Warning", "Введены не все Y практические");
}
