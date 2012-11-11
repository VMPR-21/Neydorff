#include "newexperimentdialog.h"
#include "ui_newexperimentdialog.h"

newExperimentDialog::newExperimentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newExperimentDialog)
{
    ui->setupUi(this);
}

newExperimentDialog::~newExperimentDialog()
{
    delete ui;
}
