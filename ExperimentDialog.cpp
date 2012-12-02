#include "ExperimentDialog.h"
#include "ui_ExperimentDialog.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QSlider>
#include <QSpinBox>
#include "IExperimentTable.h"
#include "INumberFormatter.h"
#include <QModelIndex>
#include "ClipCopyPaste.h"
#include "IFactorTable.h"
#include "RegressionCoefficientTable.h"
#include "bCoeff.h"
#include <QDate>
#include <QMessageBox>
#include "StudentChecker.h"

#include "NumberFormatDialog.h"
#include "ResponcesSourseFunction.h"
#include "IFactorSettingsView.h"
#include "FactorSettingsDialog.h"
#include "CenterExperimentsDialog.h"
#include "ExperimentTable.h"
#include "StudentChecker.h"
#include "BartleteCriteria.h"
#include "FisherCriteria.h"
#include "KohrenCriteria.h"
#include "ProbabilitySelectionDialog.h"
#include "OptimumSearch.h"
#include "OptimumSearchSettingsDialog.h"
#include "Experementator.h"
#include "ImaginarySteps.h"
#include "CalcFuncValueDialog.h"
#include "findExtrDialog.h"
#include <assert.h>

//#include "settingsdialog.h"
#include "NewExperimentSettingsDialog.h"


ExperimentDialog::ExperimentDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExperimentDialog)
{
    ui->setupUi(this);

    _interaction_level = 1;

    _showNormed = true;
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
    _dataSrc = NULL;
    _formatterView = new NumberFormatDialog;
    _formatter = NULL;
    _view = this;
    _modelHasData = false;
    _experimentTable = NULL;

    newModel(true);
    _dataChanged = false;

    ui->tableWidgetY->setColumnCount(2);
    ui->tableWidgetY->setRowCount(4);

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

    gg = ui->tableWidget->columnCount() - _inputs;

    ui->tableWidgetB->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidgetB, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onBtableContextMenu(QPoint)));
    ui->tableWidgetB->addAction(new QAction("My action", this));
}

void ExperimentDialog::changeBcoefSignificance(int row)
{
    const std::vector<bCoeff> &coefs = _experimentTable->b().coeffs();

    if((size_t)row >= coefs.size())
        return;

    std::vector<int> index = coefs.at(row).index();
    bool newVal = !coefs.at(row).is_significant();
    _experimentTable->b().at(index)->set_is_significant(newVal);
}

void ExperimentDialog::onBtableContextMenu(const QPoint &pt)
{
    QModelIndex index = ui->tableWidgetB->indexAt(pt);
    QTableWidgetItem *item = ui->tableWidgetB->item(index.row(), index.column());
    //    if(!item)
    //    {
    //        item=new QTableWidgetItem;
    //        ui->tableWidgetB->setItem(index.row(),index.column(),item);
    //    }
    //    item->setBackgroundColor(Qt::red);
    changeBcoefSignificance(index.row());
    dataChanged();
}


ExperimentDialog::~ExperimentDialog()
{
    delete shuffler;
    delete _formatter;
    delete _dataSrc;
    delete _formatterView;
    delete _experimentTable;
    delete ui;
}

void ExperimentDialog::startDialog(const QString &windowTitle, IExperimentTable *table, IExperimentTable *pfe_table, int interaction_level)
{
    this->setWindowTitle(windowTitle);

    _interaction_level = interaction_level;
    _table = table;

    if(_table)
    {
        _experimentTable = _table;
        updateYY(*_table);
        updateBcoefs(*_table);
        updateInputs(*_table);
    }
    else
        _table = _experimentTable;

    //дополнение ЦКП до ПФЭ
    int l = 0;
    std::vector<YInfo> y0 = pfe_table->y().at(0);

    for(size_t i = 0; i < y0.size(); i++)
        y0[i].Value = 0;

    for(int i = 0 ; i < table->x().rowCount(); i++)
    {
        for(int k = 0; k < pfe_table->x().rowCount(); k++)
        {
            bool f = true;

            for(int j = 0; j < pfe_table->x().count(); j++)
                if(pfe_table->x().norm_at(j, k) != table->x().norm_at(j, i))
                {
                    f = false;
                    break;
                }

            if(f)
            {
                table->y().set_at(i, pfe_table->y().at(k));
                l++;
                break;
            }
            else
            {
                table->y().set_at(i, y0);
            }
        }

        //if(l == pfe_table->x().rowCount())
        //    break;
    }

    for(int i = 0; i < pfe_table->x().count(); i++)
    {
        table->x().setXcenter(i, pfe_table->x().xCenter(i));
        table->x().setXdelta(i, pfe_table->x().xDelta(i));
    }

    //for(int i = table->x().rowCount(); i < ui->tableWidget->rowCount();i++)
    //    table->y().set_at(i,y0);

    updateInputs(*_table);

    this->exec();
    readData(table);
}

void ExperimentDialog::updateAdeq(const IExperimentTable &table)
{
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

/*0.2,   0.1,  0.05,  0.04,   0.03,  0.025,   0.02,   0.01,   0.005,  0.0005 */
void ExperimentDialog::Combo(int comboInt)
{
    diY = ui->comboBox->itemText(comboInt).toDouble();
}

void ExperimentDialog::Combo2(int comboInt)
{
    diB = ui->comboBox_2->itemText(comboInt).toDouble();
}

void ExperimentDialog::on_tableWidget_activated(const QModelIndex &index)
{

}

int ExperimentDialog::countColsCount(const IExperimentTable &table)
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

void ExperimentDialog::showYdata(const IExperimentTable &table)
{
    QTableWidgetItem *ptwi = NULL;

    for(int i = 0; i < table.rowCount(); ++i)
    {
        int guiRow = getGuiIdx(i);
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

    bool disable = (ui->tableWidget->columnCount() - _inputs) == 0;
    ui->pushButton->setEnabled(!disable);
}

void ExperimentDialog::updateInputs(const IExperimentTable &table)
{
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
        }
    }

    this->gg = stolbec;
    showYdata(table);
    ////////////////////////////////////////////////
    gg = ui->tableWidget->columnCount() - _inputs;

    if (gg <= 0)
    {
        ui->pushButton->setDisabled(true);
    }
    else
    {
        ui->pushButton->setDisabled(false);
    }
}

void ExperimentDialog::updateHader(const QStringList &factorDescriptions)
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
    }

    ui->tableWidget->setHorizontalHeaderLabels(l);
}

void ExperimentDialog::updateHader1()
{
    QStringList lst;
    lst << "Значение" << "Дисперсия"; //<<" Степень свободы";

    for(int i = 0; i < lst.size(); i++)
    {
        ui->tableWidgetY->setHorizontalHeaderItem(i, new QTableWidgetItem(lst[i]));
    }
}

void ExperimentDialog::updateHader2()
{
    QStringList lst;
    lst << "Название" << "? коф." << "Натуральные ?";

    for(int i = 0; i < lst.size(); i++)
    {
        ui->tableWidgetB->setHorizontalHeaderItem(i, new QTableWidgetItem(lst[i]));
    }
}

void ExperimentDialog::updateYY(const IExperimentTable &table)
{
    INumberFormatter* converter;
    converter = getNumberFormatter();
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

//    std::vector<bCoeff> bCoefs=table.b().coeffs();
//    for(int i=0;i<table.b().count(); i++)   {
//        ptwi = new QTableWidgetItem(converter->toString(bCoefs[i].norm_value())); //b coef
//        ui->tableWidgetB->setItem(i, 1, ptwi);
//        ptwi = new QTableWidgetItem(converter->toString(bCoefs[i].natural_value()));//natural B
//        ui->tableWidgetB->setItem(i, 2, ptwi);
//    }

    for(int i = 0; i < table.rowCount(); i++)
    {
    }

    updateHader1();

    showYdata(table);
}

void ExperimentDialog::updateBcoefs(const IExperimentTable &table)
{
    QTableWidgetItem* ptwi = 0;
    ui->tableWidgetB->setColumnCount(3);
    stolbec = table.x().count();

    ui->tableWidgetB->setRowCount(table.b().count());
    stroka = table.rowCount();

    ui->tableWidgetB->clear();

    INumberFormatter* converter;
    converter = getNumberFormatter();
    std::vector<bCoeff> bCoefs = table.b().coeffs();

    for(int i = 0; i < table.b().count(); i++)
    {
        ptwi = new QTableWidgetItem(converter->toString(bCoefs[i].norm_value())); //b coef
        ui->tableWidgetB->setItem(i, 1, ptwi);
        ptwi = new QTableWidgetItem(converter->toString(bCoefs[i].natural_value()));//natural B
        ui->tableWidgetB->setItem(i, 2, ptwi);

        if(bCoefs[i].is_significant())
            cellpaintB(i, 1,  bcolor);
    }

    updateHader2();

    for(int i = 0; i < table.b().count(); i++)                      //bIndexes()
    {
        QString str = "?";

        std::vector<int> bb = bCoefs[i].index();

        for(int j = 0; j < (int)bb.size(); j++)
        {
            str += QString::number(bb[j]);
        }

        ptwi = new QTableWidgetItem(str);
        ui->tableWidgetB->setItem(i, 0, ptwi);
    }
}

void ExperimentDialog::readData(IExperimentTable *table)
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

    _dataChanged = false;
}

void ExperimentDialog::on_pushButton_11_clicked()           // + stolb in Y table
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
        ui->pushButton->setDisabled(false);
    }
}

void ExperimentDialog::cellpaint(int a, int b, const QColor &color)
{
    ui->tableWidget->item(a, b)->setBackground( QBrush( color ));
}

void ExperimentDialog::cellpaintB(int a, int b, const QColor &color)
{
    ui->tableWidgetB->item(a, b)->setBackground( QBrush( color ));
}

void ExperimentDialog::cellpaintY(int a, int b, const QColor &color)
{
    ui->tableWidgetY->item(a, b)->setBackground( QBrush( color ));
}

void ExperimentDialog::on_pushButton_2_clicked()   //stolbec -
{
    gg = ui->tableWidget->columnCount() - _inputs;

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
        ui->pushButton->setDisabled(true);
    }
}

void ExperimentDialog::chModelParams()   //parametri
{
    if(_dataChanged)
        dataChanged();

    changeModelParams();
}

void ExperimentDialog::chNumberFormater() //format chisla
{
    changeNumberFormater();
}

void ExperimentDialog::nModel()  //new experement
{
    _dataChanged = false;
    newModel();
    gg = ui->tableWidget->columnCount() - _inputs;
    ui->tableWidgetY->clear();
    ui->tableWidgetB->clear();
}

void ExperimentDialog::on_tableWidget_cellChanged(int row, int column)
{
    _dataChanged = true;
}

void ExperimentDialog::on_comboBox_activated(const QString &arg1)
{

}

void ExperimentDialog::on_tableWidget_clicked(const QModelIndex &index)
{
    if(index.column() >= _inputs || index.column() < _actualInputs)
        return;

    changeFactorParams(index.column());
}

void ExperimentDialog::on_tableWidget_itemClicked(QTableWidgetItem *item)
{

}

void ExperimentDialog::on_checkBox_MixRows_toggled(bool checked)
{
    dataChanged();
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

    needRefresh(true);
}

int ExperimentDialog::getGuiIdx(int idx)
{
    if(_shuffleRows)
        return shuffler->getVirtIndex(idx);

    return idx;
}

int ExperimentDialog::getRealIdx(int guiRow)
{
    if(_shuffleRows)
        return shuffler->getRealIndex(guiRow);

    return guiRow;
}

void ExperimentDialog::checkShuffler(int rows)
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

void ExperimentDialog::on_checkBox_normNat_toggled(bool checked)
{
    dataChanged();
    _showNormed = !checked;
    needRefresh(true);
}

void ExperimentDialog::keyPressEvent(QKeyEvent *e)
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

void ExperimentDialog::on_pushButton_clicked()
{
    calcY(diY);  //(0.05) дов интервал
    calcB(diB);   ////(1.0) дов интервал
    //    StudentChecker criteria;
    //    _table->statisticProcessing(diY,criteria,_evh(this,&ExperimentDialog::centerYrequester));
    //    _experimentTable->coeffsProcessing(diB);// threshold  ???
}

void ExperimentDialog::on_pushButton_3_clicked()
{
    close();
}

//сообщает интерфейсу о необходимости обновить значения Градиента
void ExperimentDialog::updateGr(const IExperimentTable &table, const std::vector<ExperimentPoint> &points)
{

}



void ExperimentDialog::newModel(bool createEmpty)
{
    _experimentTable = NULL;
    _modelHasData = false;

    delete _dataSrc;
    _dataSrc = NULL;

    if(createEmpty)
        initEmptyModel();
    else
        changeModelParams();
}


bool ExperimentDialog::loadModel(const QString &fileName)
{
    _experimentTable->load(fileName.toAscii().data());
    bool r = _experimentTable->load(fileName.toAscii().data());

    if(r)
    {
        _view->updateBcoefs(*_experimentTable);
        //_view->updateGr(*_experimentTable);
        _view->updateInputs(*_experimentTable);
        _view->updateYY(*_experimentTable);
    }

    size_t factors = _experimentTable->x().count();
    std::vector<double> d, v;

    for(size_t i = 0; i < factors; i++)
    {
        d.push_back(_experimentTable->x().xDelta((int)i) * 2);
        v.push_back(_experimentTable->x().xCenter((int)i));
    }

    setDataSource(_experimentTable->x().generalFactorCount(), factors, v, d, _experimentTable->x().factorsDescriptions());

    _modelHasData = true;

    return r;
}

bool ExperimentDialog::saveModel(const QString &fileName)
{
    _experimentTable->save(fileName.toAscii().data());
    _view->readData(_experimentTable);
    return _experimentTable->save(fileName.toAscii().data());
}

void ExperimentDialog::calcY(double threshold)
{
    StudentChecker criteria;


    _view->readData(_experimentTable);

    _experimentTable->statisticProcessing(threshold, criteria, _evh(this, &ExperimentDialog::centerYrequester),false);
    _view->updateYY(*_experimentTable);
    _modelHasData = true;
}

void ExperimentDialog::calcB(double threshold)
{
    _view->readData(_experimentTable);
    _experimentTable->coeffsProcessing(threshold);// threshold  ???
    _view->updateBcoefs(*_experimentTable);
    _modelHasData = true;
}

void ExperimentDialog::calcExtremum(bool isMax)
{
    // CHECK CALC EXTREMUM

    _view->readData(_experimentTable);

    IExperementator *exp = new Experementator();
    bool isExtremeumFound;
    std::vector<ExperimentPoint> vals = exp->calcStepYt(isMax, *_experimentTable,
                                        _evh(this, &ExperimentDialog::experimentalResponcesRequester), &isExtremeumFound);
    delete exp;

    assert(vals.at(vals.size() - 1).xs.size() == (size_t)_experimentTable->x().count()); // xs[0] is f

    if(isExtremeumFound)
    {
        _view->updateGr(*_experimentTable, vals); // + isExtremeumFound
    }
    else
    {
        //update experiment table - we have to start new experiment in new point
        for(int i = 0; i < _experimentTable->x().count(); i++)
            _experimentTable->x().setXcenter(i, vals.at(vals.size() - 1).xs.at(i));

        //update datasrc (bad code - Refactor!)
        ResponcesSourseFunction *src = new ResponcesSourseFunction((ResponcesSourseFunction&)*_dataSrc);
        std::vector<double> v;

        for(int i = 0; i < src->FactNum; i++)
            v.push_back(_experimentTable->x().xCenter((int)i));

        src->FactValues = v;
        _dataSrc = src;
        _view->updateInputs(*_experimentTable);

        this->changeModelParams();
    }

    _modelHasData = true;
}

void ExperimentDialog::changeModelParams()
{
    IModelSettingsView *settings = new NewExperimentSettingsDialog(1,_dataSrc);
    settings->askNewSettings();
    bool saveData = _dataSrc != NULL;
    int oldInputsCount = -1, oldActualInputsCount = -1;

    if(saveData)
    {
        oldInputsCount = _dataSrc->inputsCount();
        oldActualInputsCount = _dataSrc->actualInputsCount();
    }

    if(_dataSrc)
        delete _dataSrc;

    _dataSrc = settings->getDataSource();
    _interaction_level = settings->getInteractionLevel();
    delete settings;

    //init _experimentTable from data source dataSrc
    int replicaDelim = pow(2., _dataSrc->inputsCount()) / pow(2., _dataSrc->actualInputsCount());

    if(oldInputsCount != _dataSrc->inputsCount() || oldActualInputsCount != _dataSrc->actualInputsCount())
        _experimentTable = ExperimentTable::createExperimentTable(CentralOrtogonalComposite, _dataSrc->inputsCount(), replicaDelim, _interaction_level);

    assert(_dataSrc->inputsCount() == _experimentTable->x().count());
    assert(_dataSrc->actualInputsCount() == _experimentTable->x().generalFactorCount());
    assert(_experimentTable->rowCount() >= pow(2., _dataSrc->actualInputsCount()));

    _experimentTable->x().setFactorsDescriptions(_dataSrc->getDescriptions());

    for(int i = 0; i < _dataSrc->inputsCount(); i++)
    {
        _experimentTable->x().setXcenter(i, _dataSrc->centerFor(i));
        double min, max;
        _dataSrc->intervalFor(i, &min, &max);
        _experimentTable->x().setXdelta(i, max / 2.);
    }

    if(!saveData)
        for(int i = 0; i < _experimentTable->rowCount(); i++)
        {
            std::vector<int> coords;

            for(int j = 0; j < _experimentTable->x().count(); j++)
                coords.push_back((int)_experimentTable->x().norm_at(j, i));

            std::vector<double> yy = _dataSrc->getYdata(coords); //request y data for coords.
            std::vector<YInfo> info;

            for(size_t j = 0; j < yy.size(); j++)
            {
                YInfo inf;
                inf.IsTrusted = true;
                inf.Value = yy[j];
                info.push_back(inf);
            }

            _experimentTable->y().set_at(i, info); //add it to table
        }

    _view->updateInputs(*_experimentTable);
}

void ExperimentDialog::dataChanged()
{
    IExperimentTable &t = *_experimentTable;
    _view->readData(&t);

    _modelHasData = true;
    _view->updateBcoefs(t);
    //_view->updateGr(t);
    _view->updateInputs(t);
    _view->updateYY(t);
}

INumberFormatter * ExperimentDialog::getNumberFormatter()
{
    if(_formatter)
        return _formatter;

    _formatter = _formatterView->getFormatter();
    return _formatter;
}

void ExperimentDialog::changeNumberFormater()
{
    _formatterView->askNewFormat(_formatter);
    delete _formatter;
    _formatter = _formatterView->getFormatter();

    _view->updateBcoefs(*_experimentTable);
    //_view->updateGr(*_experimentTable);
    _view->updateInputs(*_experimentTable);
    _view->updateYY(*_experimentTable);
}

void ExperimentDialog::changeFactorParams(int factorIndex)
{
    assert(factorIndex >= _experimentTable->x().generalFactorCount());
    _view->readData(_experimentTable);

    IFactorSettingsView *settings = new FactorSettingsDialog();

    if(settings->askFactorSettings(factorIndex, &_experimentTable->x()))
        _experimentTable->x().setGenerateRelation(factorIndex, settings->getSetting());

    delete settings;

    _view->updateInputs(*_experimentTable);
    _view->updateYY(*_experimentTable);
}

void ExperimentDialog::needRefresh(bool inputsOnly)
{
    _view->updateInputs(*_experimentTable);

    if(inputsOnly)
        return;

    _view->updateBcoefs(*_experimentTable);
    //_view->updateGr(*_experimentTable);
    _view->updateYY(*_experimentTable);
}

void ExperimentDialog::calcAdeq()
{
    //	FisherChecker criteria;
    //    _experimentTable->y().checkAdec(criteria,_experimentTable);//   _model->calcAdeq();
    _view->updateAdeq(*_experimentTable);
}

int ExperimentDialog::checkHomogeneity(StaticCriteria criteria, const char **strErr)
{
    std::vector<double>  probs = getProbsList(criteria);
    IProbabilitySelectionView *view = new ProbabilitySelectionDialog();
    int idx = view->askForProbability(probs);
    delete view;

    if(idx < 0)
    {
        if(strErr)
            *strErr = "Ошибка в выбранном критерии";

        return -1;
    }

    double prob = probs.at(idx);
    int r = _experimentTable->y().ongenericalDispersion(criteria, prob);

    if(r == -1 && strErr)
        *strErr = _experimentTable->y().getLastError();

    return r;
}

std::vector<double> ExperimentDialog::gradientSearch()
{
    OptimumSearchSettingsDialog dlg;
    std::vector<double> r = dlg.askAndSearch(*_experimentTable);
    return r;
}

void ExperimentDialog::calcFuncValue()
{
    CalcFuncValueDialog calcDialog;
    calcDialog.startCalcDialog(*_experimentTable);
}

void ExperimentDialog::findExtr()
{
    findExtrDialog CfindExtrDialog;
    CfindExtrDialog.startfindExtr(*_experimentTable, *_dataSrc);
}

//called when center Ys are needed.
std::vector<double> ExperimentDialog::centerYrequester(const std::vector<double> &centerVals)
{
    ICenterExperimentsView *view = new CenterExperimentsDialog();

    std::vector<double> yc;
    view->askCenterYs(yc);
    yc = view->getCenterYs();
    delete view;

    return yc;
}

std::vector<ExperimentPoint> ExperimentDialog::experimentalResponcesRequester(const std::vector<ExperimentPoint> &centerVals)
{
    assert((size_t)_experimentTable->x().count() == centerVals.at(centerVals.size() - 1).xs.size());
    std::vector<int> factsNums(_experimentTable->x().count());

    for(size_t i = 0; i < factsNums.size(); i++)
        factsNums[i] = i + 1;

    ImaginarySteps stepsDialog(factsNums, centerVals); //<vector<vector<>> + ExperimentPoints
    std::vector<ExperimentPoint> retPts = stepsDialog.getYp();
    return retPts;
}

std::vector<double> ExperimentDialog::getProbsList(StaticCriteria criteria)
{
    switch(criteria)
    {
    case Fisher:
        return FisherCriteria::SupportedProbs();

    case Kohren:
        return KohrenCriteria::SupportedProbs();

    case Bartlet:
        return BartleteCriteria::SupportedProbs();

    default:
        assert(!"this criteria not implemented!");
    }

    return std::vector<double>();
}

void ExperimentDialog::initEmptyModel()
{
    int factors = 3, baseFactors = 2;
    _experimentTable = ExperimentTable::createExperimentTable(CentralOrtogonalComposite, factors, baseFactors, _interaction_level);

    for(int i = 0; i < factors; i++)
    {
        _experimentTable->x().setXcenter(i, 0);
        _experimentTable->x().setXdelta(i, 0.5);
    }

    _view->updateInputs(*_experimentTable);

    std::vector<double> d, v;

    for(int i = 0; i < factors; i++)
    {
        d.push_back(1);
        v.push_back(0);
    }

    setDataSource(baseFactors, factors, v, d, QStringList() << "" << "" << "");
}


//make stub datasrc (bad code - Refactor!)
void ExperimentDialog::setDataSource(int baseFactors, int factorsCount, const std::vector<double> &centers, const std::vector<double> &deltas, QStringList descriptions)
{
    int sz = factorsCount - descriptions.size();

    for(int i = 0; i < sz; i++)
        descriptions.append("");

    ResponcesSourseFunction *src = new ResponcesSourseFunction();
    src->ActualFactNum = baseFactors;
    src->FactNum = factorsCount;
    //src->MaxDrobRepl=0.5;
    src->MinFactNum = -1;
    src->parall = 0;
    src->PFEnum = pow(2., src->FactNum);
    src->Descriptions = descriptions;
    src->FactDivergences = deltas;
    src->FactValues = centers;
    _dataSrc = src;
}
void ExperimentDialog::setOptimumDesabled()
{

}
