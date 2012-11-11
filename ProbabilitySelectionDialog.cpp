#include "ProbabilitySelectionDialog.h"
#include "ui_ProbabilitySelectionDialog.h"

ProbabilitySelectionDialog::ProbabilitySelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProbabilitySelectionDialog)
{
    ui->setupUi(this);
}

ProbabilitySelectionDialog::~ProbabilitySelectionDialog()
{
    delete ui;
}

int ProbabilitySelectionDialog::askForProbability(const std::vector<double> &probs)
{
    if(!probs.size())
        return -1;

    for(size_t i = 0; i < probs.size(); i++)
        ui->listWidget->addItem(QString::number(probs[i]));

    ui->listWidget->selectionModel()->select(ui->listWidget->model()->index(0, 0), QItemSelectionModel::Select);
    exec();
    return ui->listWidget->currentIndex().row();
}

void ProbabilitySelectionDialog::on_okPushButton_clicked()
{
    close();
}
