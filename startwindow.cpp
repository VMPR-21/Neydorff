#include "startwindow.h"
#include "ui_startwindow.h"

startWindow::startWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::startWindow)
{
    ui->setupUi(this);
    ActionNumber=-1;
    this->setWindowFlags(Qt::Window |
                         Qt::CustomizeWindowHint |
                         Qt::WindowTitleHint |
                         //Qt::WindowMinimizeButtonHint |
                         //Qt::WindowMaximizeButtonHint |
                         Qt::WindowCloseButtonHint |
                         Qt::WindowSystemMenuHint
                         );
}

startWindow::~startWindow()
{
    delete ui;
}

int startWindow::showStartMenu()
{
    ActionNumber=-1;
    this->exec();

    return ActionNumber;
}

void startWindow::on_clbNewExperiment_clicked()
{
    ActionNumber=0;
    close();
}

void startWindow::on_clbOpenExperiment_clicked()
{
    ActionNumber=1;
    close();
}

void startWindow::on_clbImportFromCSV_clicked()
{
    ActionNumber=2;
    close();
}
