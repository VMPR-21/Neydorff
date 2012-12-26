#include <QtGui>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtGui/QApplication>
#include <QPushButton>
#include <QApplication>
#include <QHBoxLayout>
#include <QSlider>
#include <QSpinBox>
#include "IExperimentTable.h"
#include "UiController.h"
#include "INumberFormatter.h"
#include <QModelIndex>
#include "ClipCopyPaste.h"
#include "IFactorTable.h"
#include "RegressionCoefficientTable.h"
#include "bCoeff.h"
#include <QDate>
//#include "IExperimentDialog.h"
#include "ExperimentDialog.h"
#include "ExperimentTable.h"
#include "cola.h"

#include "startwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _h = 0.5;
    _ch = 7;
    _dev = 30;

    _showNormed = true;
    control = 0; //
    xcolor = QColor(235, 235, 255);
    ycolor = QColor(255, 240, 235);
    bcolor = QColor(115, 215, 92);
    ycolorBad = QColor(255, 90, 40);
    stroka = 1;
    stolbec = 1;

    shuffler = NULL;
    _shuffleRows = false;

    _actualInputs = 0; //actual x
    _inputs = 0;
    gg = 0; //счётчик

    control = new UiController(this);

    //control->newModel(true);

    createActions();
    createMenu();

    _dataChanged = false;
    // ui->tableWidget->item(2, 1)->setBackground( QBrush( QColor(255,90,40) ) );  //отладка цвета

    //QTableWidgetItem* ptwi =0;
    ui->tableWidgetY->setColumnCount(2);
    ui->tableWidgetY->setRowCount(4);
    /*      for(int i=0; i<3; ++i){
                for(int j=0; j<4; ++j){
                    ptwi = new QTableWidgetItem(QString::number(0));
          ui->tableWidgetY->setItem(i, j, ptwi);
             }
                   }
    */
    updateHader1();
    ui->tableWidgetB->setColumnCount(3);
    ui->tableWidgetB->setRowCount(4);
    updateHader2();

    diY = diB = 0.2;


    QStringList lst;
    lst << "0.2" << "0.1" << "0.05" << "0.04" << "0.03" << "0.025" << "0.02" << "0.01" << "0.005" << "0.0005";
    ui->comboBox->addItems(lst);
    ui->comboBox_2->addItems(lst);
    connect( ui->comboBox, SIGNAL(activated(int)), this, SLOT(Combo(int)));
    connect( ui->comboBox_2, SIGNAL(activated(int)), this, SLOT(Combo2(int)));


    // int r;
    //_inputs=0;
    gg = ui->tableWidget->columnCount() - _inputs;

    QMenu *btnMenu = new QMenu();
    connect(btnMenu, SIGNAL(triggered(QAction*)), this, SLOT(onHomogeneityBtnAction(QAction*)));
    btnMenu->addAction("Фишер")->setData(Fisher);
    btnMenu->addAction("Бартлет")->setData(Bartlet);
    btnMenu->addAction("Кохрен")->setData(Kohren);
    ui->homogeneity_Button->setMenu(btnMenu);

    connect(&clockTimer, SIGNAL(timeout()), this, SLOT(onClockTimer()));
    clockTimer.start(1000);

    ui->tableWidgetB->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidgetB, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onBtableContextMenu(QPoint)));
    ui->tableWidgetB->addAction(new QAction("My action", this));

ui->pushButton_9->hide();
ui->pushButton_15->hide();
ui->pushButton_MIN->hide();
ui->pushButton_MAX->hide();


    startWindow * startwin =new startWindow();

    bool res=false;
    startProgram=true;
    while (!res)
    {
        int Action=startwin->showStartMenu();

        switch (Action)
        {
        case -1:
        {
            startProgram=false;
            res=true;
            break;
        }
        case 0:
        {
            res= control->newModel(MATHMODEL);
            this->ui->findExtrButton_3->setEnabled(false);
            ModelType=MATHMODEL;
            break;
        }
        case 1:
        {
            res= control->newModel(EXTRSEARCH);
            this->ui->findExtrButton_3->setEnabled(true);
            ModelType= EXTRSEARCH;
            break;
        }
        case 2:
        {
            res=loadToFile();
            break;
        }
        case 3:
        {
            res=NewExperimenLoadfromCSV();
            break;
        }
        }
    }

updateHader(_factorDescriptions);
ui->tableWidget->horizontalHeader()->setVisible(true);

}


MainWindow::~MainWindow()
{
    delete shuffler;
    delete control;

    delete ui;
}
/*0.2,   0.1,  0.05,  0.04,   0.03,  0.025,   0.02,   0.01,   0.005,  0.0005 */
void MainWindow::Combo(int comboInt)
{
    diY = ui->comboBox->itemText(comboInt).toDouble();

    //    if(comboInt == 0){
    //        di=0.2;
    //        QMessageBox::information(this, ("Сообщение"), ("Num 0.05"));
    //    }
    //        if(comboInt == 1){

    //            QMessageBox::information(this, ("Сообщение"), ("Num 2"));
    //        }
    //        if(comboInt == 2){
    //            QMessageBox::information(this, ("Сообщение"), ("Num 3"));
    //        }
}

void MainWindow::Combo2(int comboInt)
{
    diB = ui->comboBox_2->itemText(comboInt).toDouble();
}

void MainWindow::on_pushButton_clicked()
{
    close();
    // QObject::connect(ui->pushButton, SIGNAL(clicked()),QApplication::instance(), SLOT(quit()));
    // QMessageBox::information(this, tr("Сообщение"), tr("Благодарим за использование нашей программы!"));
    //  QMessageBox::aboutQt(0);
}

void MainWindow::on_tableWidget_activated(const QModelIndex &index)
{

}


// ╬   createAction    ╬
void MainWindow::createActions()
{
    saveActions = new QAction(("&Сохранить"), this);
    saveActions->setShortcut(tr("Ctrl+S"));
    // saveActions setIcon(QIcon(""));
    // connect(newAction, SIGNAL(ggggg()), this, SLOT(save()));
    connect(saveActions, SIGNAL(triggered()), this, SLOT(saveToFile()));

    openActions = new QAction(("&Открыть"), this);
    openActions->setShortcut(tr("Ctrl+O"));
    connect(openActions, SIGNAL(triggered()), this, SLOT(loadToFile()));

    saveToCSVActions = new QAction(("&Экспорт в CSV"), this);
//    saveToCSVActions->setShortcut(tr("Ctrl+S"));
    connect(saveToCSVActions, SIGNAL(triggered()), this, SLOT(saveToCSV()));

    openFromCSVActions = new QAction(("&Импорт из CSV"), this);
//    openFromCSVActions->setShortcut(tr("Ctrl+O"));
    connect(openFromCSVActions, SIGNAL(triggered()), this, SLOT(loadFromCSV()));

    newActions = new QAction(("&Новый эксперимент.Моделирование"), this);
    newActions->setShortcut(tr("Ctrl+N"));
    connect(newActions, SIGNAL(triggered()), this, SLOT(nModel()));

    newActionsE = new QAction(("&Новый эксперимент.Поиск экстремума"), this);
    newActionsE->setShortcut(tr("Ctrl+E"));
    connect(newActionsE, SIGNAL(triggered()), this, SLOT(nModelExtr()));

    /*
    newActionsF = new QAction(("&Новый эксперимент(формула)"), this);
    newActionsF->setShortcut(tr("Ctrl+N"));
    connect(newActionsF, SIGNAL(triggered()), this, SLOT(nModelFormula()));
    */

    paramsAction = new QAction(("&Исходные настройки эксперимента"), this);
    connect(paramsAction, SIGNAL(triggered()), this, SLOT(chModelParams()));

    savepathAction = new QAction(("&Расположение сохраняемых данных"), this);
    connect(savepathAction, SIGNAL(triggered()), this, SLOT(ChFileFolder()));


    formatChisAction = new QAction(("&Формат числа"), this);
    connect(formatChisAction, SIGNAL(triggered()), this, SLOT(chNumberFormater()));

    aboutQtAction = new QAction (("О программе"), this);
    aboutQtAction->setStatusTip(("aaa"));
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


    testExperimentTableMaxAction = new QAction(("&Тестировать Experiment Table Max"), this);
    connect(testExperimentTableMaxAction, SIGNAL(triggered()), this, SLOT(testExperimentTableSlotMax()));

    testExperimentTableMinAction = new QAction(("&Тестировать Experiment Table Min"), this);
    connect(testExperimentTableMinAction, SIGNAL(triggered()), this, SLOT(testExperimentTableSlotMin()));

    testSomeProcessAction = new QAction(("&Процесс"), this);
    connect(testSomeProcessAction, SIGNAL(triggered()), this, SLOT(testSomeProcessSlot()));

/*
    colaSettingsAction = new QAction(("Параметры поиска экстремума"), this);
    connect(colaSettingsAction, SIGNAL(triggered()), this, SLOT(colaSettingsSlot()));
*/
}

// ╬    createMenu    ╬
void MainWindow::createMenu()
{
    fileMenu = menuBar()->addMenu("&Файл");
    fileMenu->addAction(newActions);
    fileMenu->addAction(newActionsE);
    fileMenu->addAction(openActions);
    fileMenu->addAction(saveActions);
    fileMenu->addAction(openFromCSVActions);
    fileMenu->addAction(saveToCSVActions);

    // seporatorAction = fileMenu->addSeparator();
    //for(int i=0; i < MaxRecentFiles; ++i)
    // fileMenu->addAction(recentFileActions[i]);
    //   fileMenu->addSeparator();

    editMenu = menuBar()->addMenu("&Правка");
    editMenu->addAction(formatChisAction);

    optionMenu = menuBar()->addMenu("&Опции");
    optionMenu->addAction(paramsAction);
    optionMenu->addAction(savepathAction);
    //optionMenu->addAction(colaSettingsAction);

    helpMenu = menuBar()->addMenu("&Справка");
    helpMenu->addAction(aboutQtAction);

    /*
    testMenu = menuBar()->addMenu("&Тест");
    testMenu->addAction(testExperimentTableMaxAction);
    testMenu->addAction(testExperimentTableMinAction);
    testMenu->addAction(testSomeProcessAction);
    */
}

/*
// ╬   createToolBars   ╬
void MainWindow::createToolBars()
{
    fileToolBar=addToolBar(("&File"));
   // fileToolBar->addAction(OpenAction);
    fileToolBar->addAction(saveAction);
}
*/

int MainWindow::countColsCount(const IExperimentTable &table)
{
    IResponseTable &ys = table.y();
    size_t colsCount = 0;

    for(int i = 0; i < ys.count(); i++)
    {
        size_t sz = ys.at(i).size();

        if(colsCount < sz)
            colsCount = sz;
    }

    return colsCount + table.x().count();
}

void MainWindow::showYdata(const IExperimentTable &table)
{
    QTableWidgetItem *ptwi = NULL;

    //std::vector<std::vector<double> >   bd=table.badYvals();
    for(int i = 0; i < table.rowCount(); ++i)
    {
        ui->tableWidget->resizeColumnToContents(i);
        int guiRow = getGuiIdx(i);
        //std::vector<double> data=table.yData(i);
        std::vector<YInfo> data = table.y().at(i);

        for(int j = 0; j < (int)data.size(); ++j)
        {
            ptwi = new QTableWidgetItem(QString::number(data[j].Value, 'g', 15));
            ui->tableWidget->setItem(guiRow, j + table.x().count(), ptwi); //all mixed

            if(!data[j].IsTrusted)
                cellpaint(guiRow, table.x().count() + j, ycolorBad); //подцветка выбросов
        }
    }

    for(int i = 0; i < _inputs; i++)
        ui->tableWidget->resizeColumnToContents(i);

    //int cc=ui->tableWidget->columnCount();
    bool disable = (ui->tableWidget->columnCount() - _inputs) == 0;
    ui->pushButton_5->setEnabled(!disable);
    ui->pushButton_6->setEnabled(!disable);
    // ui->tableWidget->resizeColumnsToContents();
}

//  ╬ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ ╬
void MainWindow::updateInputs(const IExperimentTable &table)
{
    // gg=ui->tableWidget->columnCount()-_inputs;
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(0);
    QTableWidgetItem* ptwi = 0;
    xcount = table.x().count();

    ui->tableWidget->setColumnCount(countColsCount(table));
    stolbec = table.x().count();

    _actualInputs = table.x().generalFactorCount();
    _inputs = table.x().count();

    checkShuffler(table.rowCount());

    ui->tableWidget->setRowCount(table.rowCount());
    stroka = table.rowCount();

    _factorDescriptions = table.x().factorsDescriptions();

    updateHader(_factorDescriptions); //text horizontal/vertical

    for(int i = 0; i < table.rowCount(); i++)
    {
        for(int j = 0; j < table.x().count(); j++)
        {
            double xi = _showNormed ? table.x().norm_at(j, i) : table.x().natural_at(j, i);

            ptwi = new QTableWidgetItem(QString::number(xi));
            ptwi->setFlags(ptwi->flags()& ~Qt::ItemIsEditable);// нельзя редактировать

            // ui->tableWidget->horizontalHeaderItem(j)->setSizeHint(QSize(11,44/*ui->tableWidget->horizontalHeaderItem(j)->sizeHint().height()*/));
            if(j >= _actualInputs)
            {
                QString tip = "=";
                std::vector<int> fi = table.x().getGenerateRelation(j);

                for(size_t k = 0; k < fi.size(); k++)
                {
                    if(fi[k] < 0)
                        tip += "-";

                    tip += "X" + QString::number(abs(fi[k])) + "*";
                }

                if(tip.size() && tip[tip.size() - 1] == '*')
                    tip.chop(1);

                ui->tableWidget->horizontalHeaderItem(j)->setToolTip(tip);
            }

            ui->tableWidget->setColumnWidth(j, 36);
            int guiRow = getGuiIdx(i);
            ui->tableWidget->setVerticalHeaderItem(guiRow, new QTableWidgetItem(QString::number(i + 1)));
            ui->tableWidget->setItem(guiRow, j, ptwi);

            //cellpaint(guiRow, j, xcolor);
            if (j < _actualInputs)
                cellpaint(guiRow, j, xcolor);
            else
                cellpaint(guiRow, j, ycolor);
        }
    }

    for(int i = 0; i < table.rowCount(); i++)
    {
        std::vector<YInfo> yy = table.y().at(i);

        if((int)yy.size() + xcount > stolbec)
        {
            stolbec = xcount + yy.size();
            ui->tableWidget->setColumnCount(stolbec);
        }

        for(int j = 0; j < (int)yy.size(); j++)
        {
            ptwi = new QTableWidgetItem(QString::number(yy[j].Value));
            ui->tableWidget->setItem(i, j + xcount, ptwi);
            //cellpaint(i, j+xcount, ycolor);
        }
    }

    this->gg = stolbec;
    showYdata(table);
    ////////////////////////////////////////////////
    gg = ui->tableWidget->columnCount() - _inputs;

    if (gg <= 0)
    {
        ui->pushButton_5->setDisabled(true);
        ui->pushButton_6->setDisabled(true);
        ui->calcFunc_btn->setDisabled(true);
        ui->pushButton_8->setDisabled(true);
        ui->homogeneity_Button->setDisabled(true);
   //     ui->gradSearch_Button->setDisabled(true);
    //    ui->pushButton_9->setDisabled(true);
    //    ui->pushButton_15->setDisabled(true);

   //     ui->pushButton_MAX->setDisabled(true);
   //     ui->pushButton_MIN->setDisabled(true);
        ui->pushButton_CCP->setDisabled(true);
        ui->findExtrButton_3->setDisabled(true);
    }
    else
    {
        ui->pushButton_5->setDisabled(false);
        ui->pushButton_6->setDisabled(false);
        ui->calcFunc_btn->setDisabled(false);
        ui->pushButton_8->setDisabled(false);
        ui->homogeneity_Button->setDisabled(false);
        //ui->gradSearch_Button->setDisabled(false);
       // ui->pushButton_9->setDisabled(false);
      //  ui->pushButton_15->setDisabled(false);

       // ui->pushButton_MAX->setDisabled(false);
      //  ui->pushButton_MIN->setDisabled(false);
        ui->pushButton_CCP->setDisabled(false);
        ui->findExtrButton_3->setDisabled(false);

        ui->tableWidgetB->clear();
        ui->tableWidgetY->clear();
    }

    /////////////////////////////////////////////////
}

void MainWindow::updateHader(const QStringList &factorDescriptions)
{
    QStringList l;

    for(int i = 0; i < ui->tableWidget->columnCount(); i++)
    {
        QString text = i < xcount ? "x" + QString::number(i + 1) : "y" + QString::number(i - xcount + 1);
        QString s;

        if(i < xcount && i < factorDescriptions.size() && !(s = factorDescriptions[i].trimmed()).isEmpty())
        {
            //add description of factor
            text += "\n" + s;
        }

        l << text;
        //ui->tableWidget->setHorizontalHeaderItem(i,new QTableWidgetItem(text));

    }

    ui->tableWidget->setHorizontalHeaderLabels(l);
    // ui->tableWidget->resizeColumnsToContents();
    //ui->tableWidget->resizeRowsToContents();
}

void MainWindow::updateHader1()
{
    QStringList lst;
    lst << "Значение" << "Дисперсия"; //<<" Степень свободы";

    for(int i = 0; i < lst.size(); i++)
    {
        ui->tableWidgetY->setHorizontalHeaderItem(i, new QTableWidgetItem(lst[i]));
    }
}

void MainWindow::updateHader2()
{
    QStringList lst;
    lst << "Название" << "β коф." << "Натуральные β";

    for(int i = 0; i < lst.size(); i++)
    {
        ui->tableWidgetB->setHorizontalHeaderItem(i, new QTableWidgetItem(lst[i]));
    }
}

void MainWindow::setOptimumDesabled()   //parametri
{
    ui->findExtrButton_3->setEnabled(false);
}

void MainWindow::updateYY(const IExperimentTable &table)
{
    INumberFormatter* converter;
    converter = control->getNumberFormatter();

    QTableWidgetItem* ptwi = 0;
    ui->tableWidgetY->setColumnCount(2);
    stolbec = table.x().count();

    ui->tableWidgetY->setRowCount(table.rowCount());
    stroka = table.rowCount();

    ui->tableWidgetY->clear();

    for(int i = 0; i </*table.rowCount()*/table.y().count(); i++)
    {

        int chisy = table.y().trusted_values_at(i).size();

        if(!chisy)
            continue;

        ptwi = new QTableWidgetItem(converter->toString(table.y().trusted_average_at(i)) + "/ " + QString::number(chisy)); //sred znachen

        ui->tableWidgetY->setItem(i, 0, ptwi);
        cellpaintY(i, 0, ycolor);

        ptwi = new QTableWidgetItem(converter->toString(table.y().dispersion(i)/*+"/ "+QString::number(table.freedomDegrees(i))*/));//  disp
        ui->tableWidgetY->setItem(i, 1, ptwi);

    }


    std::vector<bCoeff> bCoefs = table.b().coeffs();

    for(int i = 0; i < table.b().count(); i++)
    {
        ptwi = new QTableWidgetItem(converter->toString(bCoefs[i].norm_value())); //b coef
        ui->tableWidgetB->setItem(i, 1, ptwi);
        ptwi = new QTableWidgetItem(converter->toString(bCoefs[i].natural_value()));//natural B
        ui->tableWidgetB->setItem(i, 2, ptwi);
    }

    for(int i = 0; i < table.rowCount(); i++)
    {
        //ptwi = new QTableWidgetItem("/"+QString::number(table.freedomDegrees(i)));   //step svobod
        //  ui->tableWidgetY->setItem(i, 2, ptwi);
    }

    updateHader1();

    showYdata(table);
}

///////////////////////////////////////////////////////////////////////////////////////////

void MainWindow::updateBcoefs(const IExperimentTable &table)
{
    QTableWidgetItem* ptwi = 0;
    ui->tableWidgetB->setColumnCount(3);
    stolbec = table.x().count();

    ui->tableWidgetB->setRowCount(table.b().count());
    stroka = table.rowCount();

    ui->tableWidgetB->clear();

    //  if(!ui->pushButton_6->isEnabled())//if we cant't calc. B coefs than we dont't have any valid
    //      return;

    INumberFormatter* converter;
    converter = control->getNumberFormatter();

    const std::vector<bCoeff> &bCoefs = table.b().coeffs();

    for(int i = 0; i < table.b().count(); i++)
    {
        ptwi = new QTableWidgetItem(converter->toString(bCoefs[i].norm_value())); //b coef
        ui->tableWidgetB->setItem(i, 1, ptwi);
        ptwi = new QTableWidgetItem(converter->toString(bCoefs[i].natural_value()));//natural B

        if(bCoefs[i].is_significant())
            cellpaintB(i, 1,  bcolor);
        else
            cellpaintB(i, 1, Qt::white);

        ui->tableWidgetB->setItem(i, 2, ptwi);


    }

    updateHader2();

    //std::vector<std::vector<int> > xx=table.bIndexes();
    for(int i = 0; i < table.b().count(); i++)                      //bIndexes()
    {
        QString str = "β";

        std::vector<int> bb = bCoefs[i].index();

        for(int j = 0; j < (int)bb.size(); j++)
        {
            str += QString::number(bb[j]);
            //str+=" ";
        }

        ptwi = new QTableWidgetItem(str);
        ui->tableWidgetB->setItem(i, 0, ptwi);
        // cellpaint(i, j, xcolor);
    }
}

void MainWindow::updateGr(const IExperimentTable &table, const vector<ExperimentPoint> &points)
{
    ui->listWidget_4->clear();

    //std::vector<DataTable::Point> points = table.steps();
    for (int i = 0; i < (int)points.size(); ++i)
    {
        QString str = "(";

        for(size_t j = 0; j < points[i].xs.size(); ++j)
        {
            str += "" + QString::number(points[i].xs[j]);
            str += ", ";
        }

        str.chop(2);
        str += ") = ";
        str += QString::number(points[i].yp);
        //ui->listWidget_4->addItem(str);
    }

    //        QString grt="(";
    //        std::vector<double> gr=table.gradData();
    //        for(int j=0; j<gr.size(); ++j){
    //            grt += "" + QString::number(gr[j]);
    //            grt +=", ";
    //        }
    //        grt.chop(2);
    //        grt+=")";
    //        if(gr.size())
    //            ui->lineEdit->setText(grt);

    if(ui->listWidget_4->count())
        ui->lineEdit_2->setText(ui->listWidget_4->item(ui->listWidget_4->count() - 1)->text());

    // if(ui->listWidget_4->count()==0 || table.isGradFound()==0)
    //     ui->lineEdit_2->setText("Экстремум не найден");
}

void MainWindow::readData(IExperimentTable *table)
{
    for (int i = 0; i < table->rowCount(); ++i)
    {
        std::vector<YInfo> vals;

        int guiRow = getGuiIdx(i);

        for (int j = table->x().count(); j < ui->tableWidget->columnCount(); ++j)
        {
            YInfo yy;
            QTableWidgetItem *ptwi  = ui->tableWidget->item(guiRow, j);

            if(ptwi != NULL)
            {
                bool isOk;
                yy.Value = ptwi->text().toDouble(&isOk);

                if(!isOk)
                    continue;

                if (ptwi->backgroundColor() == ycolorBad)
                    yy.IsTrusted = false;
                else
                    yy.IsTrusted = true;

                vals.push_back(yy);
            }
        }

        table->y().set_at(i, vals);
        vals.clear();
    }

    //table->setBadYvals(std::vector<std::vector<double> >());
    _dataChanged = false;
}

//  ╬  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  ╬

void MainWindow::on_pushButton_10_clicked()    //+stroka
{
    stroka = stroka + 1;

    for (int i = 0; i <= stroka; ++i)
    {
        ui->tableWidget->setRowCount(stroka);
    }
}

void MainWindow::on_pushButton_11_clicked()           // + stolb in Y table
{
    gg = ui->tableWidget->columnCount() - _inputs;
    QTableWidgetItem* ptwi = 0;
    stolbec = stolbec + 1;
    ui->tableWidget->setColumnCount(ui->tableWidget->columnCount() + 1);

    gg++; //schetchik


    for(int j = 0; j < stroka; ++j)
    {
        ptwi = new QTableWidgetItem(QString::number(0));
        ui->tableWidget->setItem(j, ui->tableWidget->columnCount() - 1, ptwi);
    }

    updateHader(_factorDescriptions);

    if (gg > 0)
    {
        ui->pushButton_5->setDisabled(false);
        ui->pushButton_6->setDisabled(false);
        ui->calcFunc_btn->setDisabled(false);
        ui->pushButton_8->setDisabled(false);
        ui->homogeneity_Button->setDisabled(false);

        ui->pushButton_CCP->setDisabled(false);
        ui->findExtrButton_3->setDisabled(false);

        ui->tableWidgetB->clear();
        ui->tableWidgetY->clear();
    }

}

void MainWindow::cellpaint(int a, int b, const QColor &color)
{
    ui->tableWidget->item(a, b)->setBackground( QBrush( color ));

}

void MainWindow::cellpaintB(int a, int b, const QColor &color)
{
    ui->tableWidgetB->item(a, b)->setBackground( QBrush( color ));

}
void MainWindow::cellpaintY(int a, int b, const QColor &color)
{
    ui->tableWidgetY->item(a, b)->setBackground( QBrush( color ));


}
void MainWindow::on_pushButton_12_clicked()
{
    QTableWidgetItem* ptwi = 0;
    QString StringItem;


    ptwi  = ui->tableWidget->item(1, 1);

    if ( ptwi != 0 )
    {
        StringItem = ptwi->text();
        ptwi = new QTableWidgetItem(QString(StringItem).arg(2).arg(2));
        ui->tableWidget->setItem(2, 2, ptwi);

        //cellpaint(1, 1, xcolor);
        //cellpaint(2, 2, ycolor);

        /*
        ui->tableWidget->item(1, 1)->setBackground( QBrush( QColor(255,240,235) ) );
        ui->tableWidget->item(2, 1)->setBackground( QBrush( QColor(200,240,235) ) );
        */
    }
}

void MainWindow::on_pushButton_5_clicked()
{

    control->calcY(diY);  //(0.05) дов интервал
}

void MainWindow::on_pushButton_6_clicked()
{
    // ui->pushButton_5->setDisabled(true);
    control->calcB(diB);   ////(1.0) дов интервал
}

void MainWindow::on_pushButton_ChangeParams_clicked()
{
    this->hide();
    control->changeModelParams();

    this->show();
}



void MainWindow::on_pushButton_2_clicked()   //stolbec -
{
    gg = ui->tableWidget->columnCount() - _inputs;

    //control->changeNumberFormater();
    if(ui->tableWidget->columnCount() == _inputs)
        return;

    if (gg >= 1)
    {
        stolbec = stolbec - 1;
        ui->tableWidget->setColumnCount(ui->tableWidget->columnCount() - 1);
        updateHader(_factorDescriptions);
    }

    if(gg > 0)
    {
        gg--;
    }

    if (gg <= 0)
    {
        ui->pushButton_5->setDisabled(true);
        ui->pushButton_6->setDisabled(true);
        ui->calcFunc_btn->setDisabled(true);
        ui->pushButton_8->setDisabled(true);
        ui->homogeneity_Button->setDisabled(true);

        ui->pushButton_6->setDisabled(true);
        ui->pushButton_9->setDisabled(true);
        ui->pushButton_15->setDisabled(true);
        ui->pushButton_MAX->setDisabled(true);
        ui->pushButton_MIN->setDisabled(true);
        ui->pushButton_CCP->setDisabled(true);

        ui->findExtrButton_3->setDisabled(true);
    }
}

// save
void MainWindow::saveToFile()
{
    _dataChanged = false;
    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
    QString path="";
    path=settings->value("settings/examle_A_S").toString();

    QString fileName = QFileDialog::getSaveFileName(this, ("Сохранить"), path, ("Table (*.a_s);;All Files(*)")); //QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)
    control->saveModel(fileName);
}

// save to CSV
void MainWindow::saveToCSV()
{
    _dataChanged = false;
    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
    QString path="";
    path=settings->value("settings/examle_CSV").toString();

    QString fileName = QFileDialog::getSaveFileName(this, ("Сохранить"), path /*"new_experiment.csv"*/, ("CSV(*.csv);;All Files(*)")); //QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)
    control->saveModel(fileName);
}

//load

bool MainWindow::loadToFile()
{
    _dataChanged = false;
    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
    QString path="";
    path=settings->value("settings/examle_A_S").toString();
    QString fileName = QFileDialog::getOpenFileName(this, ("Открыть"), path, ("Table (*.a_s);;All Files(*)")); //QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)
   return control->loadModel(fileName);
}

//load from CSV

bool MainWindow::loadFromCSV()
{
    _dataChanged = false;
    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
    QString path="";
    path=settings->value("settings/examle_CSV").toString();

    QString fileName = QFileDialog::getOpenFileName(this, ("Открыть"), path, ("CSV(*.csv);;All Files(*)")); //QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)
   return control->NewExperimenLoadfromCSV(fileName);
}

bool MainWindow::NewExperimenLoadfromCSV()
{
    _dataChanged = false;
    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
    QString path="";
    path=settings->value("settings/examle_CSV").toString();
    QString fileName = QFileDialog::getOpenFileName(this, ("Импорт из CSV"), path, ("CSV(*.csv);;All Files(*)")); //QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)
   return control->NewExperimenLoadfromCSV(fileName);
}

void MainWindow::chModelParams()   //parametri
{
    if(_dataChanged)
        control->dataChanged();

    control->changeModelParams();

    bool res=ModelType==1? false:true;
    this->ui->findExtrButton_3->setEnabled(res);
}

void MainWindow::chNumberFormater() //format chisla
{
    control->changeNumberFormater();
}
void MainWindow::nModel()  //new experement
{
    _dataChanged = false;
    this->hide();
    control->newModel(1,false);
    gg = ui->tableWidget->columnCount() - _inputs;
    ui->tableWidgetY->clear();
    ui->tableWidgetB->clear();
    ui->listWidget_4->clear();
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->findExtrButton_3->setEnabled(false);
    this->show();
}

void MainWindow::nModelExtr()
{
    _dataChanged = false;
    control->newModel(2,false, true);
    gg = ui->tableWidget->columnCount() - _inputs;
    ui->tableWidgetY->clear();
    ui->tableWidgetB->clear();
    ui->listWidget_4->clear();
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->findExtrButton_3->setEnabled(true);
    this->show();
}

void MainWindow::ChFileFolder()
{
        control->ChSaveFolderPath();
}

void MainWindow::on_pushButton_3_clicked()  //load
{
    _dataChanged = false;
    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
    QString path="";
    path=settings->value("settings/examle_A_S").toString();
    QString fileName = QFileDialog::getOpenFileName(this, ("Load"), path, ("Table (*.a_s);;All Files(*)"));
    control->loadModel(fileName);
}

void MainWindow::on_pushButton_4_clicked() //save
{
    _dataChanged = false;
    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
    QString path="";
    path=settings->value("settings/examle_A_S").toString();
    QString fileName = QFileDialog::getSaveFileName(this, ("Save"), path, ("Table (*.a_s);;All Files(*)"));
    control->saveModel(fileName);
}

void MainWindow::on_pushButton_8_clicked()    //adekvatnost
{
    control->calcAdeq();
}

void MainWindow::on_pushButton_9_clicked()    //max exstrem
{
    //control->calcExtremum(true);
}

void MainWindow::on_pushButton_15_clicked()  //min exstrem
{
    //control->calcExtremum(false);
}

void MainWindow::on_pushButton_16_clicked()
{
    _dataChanged = false;
    control->newModel();
}

void MainWindow::on_tableWidget_cellChanged(int row, int column)
{
    _dataChanged = true;
}



void MainWindow::on_comboBox_activated(const QString &arg1)
{

}


void MainWindow::on_tableWidget_clicked(const QModelIndex &index)
{
    if(index.column() >= _inputs || index.column() < _actualInputs)
        return;

    control->changeFactorParams(index.column());
}

void MainWindow::on_tableWidget_itemClicked(QTableWidgetItem *item)
{

}

void MainWindow::on_checkBox_MixRows_toggled(bool checked)
{
    control->dataChanged();
    _shuffleRows = checked;

    if(checked && shuffler)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, " Warning! ",
                                      "Восстановить предыдущую рандомизацию? ",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes)
        {
        }
        else
        {
            delete shuffler;
            shuffler = NULL;
            shuffler = new Shuffler(ui->tableWidget->rowCount());
            shuffler->shuffle();
        }
    }

    control->needRefresh(true);
}

//shuffite

int MainWindow::getGuiIdx(int idx)
{
    if(_shuffleRows)
        return shuffler->getVirtIndex(idx);

    return idx;
}

int MainWindow::getRealIdx(int guiRow)
{
    if(_shuffleRows)
        return shuffler->getRealIndex(guiRow);

    return guiRow;
}

void MainWindow::checkShuffler(int rows)
{
    if(_shuffleRows)
    {
        if(shuffler && shuffler->count() != rows)
        {
            delete shuffler;
            shuffler = NULL;
        }

        if(!shuffler)
        {
            shuffler = new Shuffler(rows);
            shuffler->shuffle();
        }

    }
}

void MainWindow::updateAdeq(const IExperimentTable &table)
{
    //    QString text;
    //    int r=table.isEquationAdequate();
    //    QString rText;
    //    if(r==1)
    //       rText="Aдекватно";
    //    else if(r==0)
    //    {
    //        rText="Не адекватно";
    //    }
    //    else
    //    {
    //        rText="Неизвестно";
    //    }
    //    text=QString("Дисперсия адекватности: %1\nДисперсия воспроизводимости: %2\nАдекватность уравнения: %3")
    //            .arg(table.dispAdekv()).arg(table.dispVospr()).arg(rText);
    //    QMessageBox::information(0,"Результаты проверки на адекватность",text);

    bool r = table.y().checkAdec(table);
    QString rText;

    if(r)
        rText = "Aдекватно";
    else// if(r==0)
    {
        rText = "Не адекватно";
    }

    QMessageBox::information(0, "Результаты проверки на адекватность", rText);
}

void MainWindow::onBtableContextMenu(const QPoint &pt)
{
    QModelIndex index = ui->tableWidgetB->indexAt(pt);
    QTableWidgetItem *item = ui->tableWidgetB->item(index.row(), index.column());
    //    if(!item)
    //    {
    //        item=new QTableWidgetItem;
    //        ui->tableWidgetB->setItem(index.row(),index.column(),item);
    //    }
    //    item->setBackgroundColor(Qt::red);
    control->changeBcoefSignificance(index.row());
    control->dataChanged();
}

void MainWindow::onHomogeneityBtnAction(QAction *action)
{
    StaticCriteria crit = (StaticCriteria)action->data().toInt();
    const char *str;
    int r = control->checkHomogeneity(crit, &str);
    QString rText;

    if(r == 1)
        rText = "Однородно";
    else if(r == 0)
    {
        rText = "Не однородно";
    }
    else
    {
        rText = str;
    }

    QMessageBox::information(0, "Результаты проверки на однородность", rText);
}

void MainWindow::on_checkBox_normNat_toggled(bool checked)
{
    control->dataChanged();
    _showNormed = !checked;
    control->needRefresh(true);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    QTableWidget *tableWidget = ui->tableWidget;

    if(ui->tableWidgetY->hasFocus())
        tableWidget = ui->tableWidgetY;

    if(ui->tableWidgetB->hasFocus())
        tableWidget = ui->tableWidgetB;

    if(e->type() == QKeyEvent::KeyPress)
    {
        if(e->matches(QKeySequence::Copy))
        {
            ClipCopyPaste ccp;
            ccp.clipCopy(tableWidget);
        }
        else if (e->matches((QKeySequence::Paste)))
        {
            ClipCopyPaste ccp;
            ccp.clipPaste(tableWidget);
        }
    }
}

void MainWindow::on_homogeneity_Button_clicked()
{
}

void MainWindow::onClockTimer()
{
    ui->lcdNumber->display(QTime::currentTime().toString());
    //setWindowTitle(QTime::currentTime().toString());
}

void MainWindow::on_gradSearch_Button_clicked()
{
    /*
    std::vector<double> r = control->gradientSearch();
    QString s;

    for(size_t i = 0; i < r.size(); i++)
        s.append(QString::number(r[i]) + ", ");

    s.chop(2);// last ", "
    QMessageBox::information(this, "Вектор-результат градиентного поиска", "X={" + s + "}");
    */
}

void MainWindow::on_calcFunc_btn_clicked()
{
    control->calcFuncValue();
}

void MainWindow::testExperimentTable(bool max, double _h, int _ch, int _dev)
{
    control->startProcess(max, _h, _ch, _dev);
}

void MainWindow::testExperimentTableSlotMax()
{
    testExperimentTable(true, _h, _ch, _dev);
}

void MainWindow::testExperimentTableSlotMin()
{
    testExperimentTable(false, _h, _ch, _dev);
}

void MainWindow::testSomeProcessSlot()
{

}

void MainWindow::on_pushButton_MAX_clicked()
{
    //testExperimentTable(true, _h, _ch, _dev);
}

void MainWindow::on_pushButton_MIN_clicked()
{
    //testExperimentTable(false, _h, _ch, _dev);
}

void MainWindow::colaSettingsSlot()
{
    Cola * c = new Cola();
    c->getParams(_h, _ch, _dev);
}

void MainWindow::on_pushButton_CCP_clicked()
{
    control->CCP();
}

void MainWindow::on_findExtrButton_3_clicked()
{
    control->calcB(diB);
    control->findExtr();
}
