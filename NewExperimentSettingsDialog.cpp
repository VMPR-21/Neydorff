#include "NewExperimentSettingsDialog.h"

#include "ExperimentTable.h"
#include "DataStream.h"
#include <QFile>
#include <QTextStream>
#include <assert.h>
#include "bCoeff.h"
#include "GradientFactorTable.h"
#include "GradientRegressionCoefficientTable.h"
#include "CentralOrtogonalFactorTable.h"
#include "CentralOrtogonalRegressionCoefficientTable.h"
#include "ResponseTable.h"
#include "math.h"
#include <QTextCodec>


#ifdef _MSC_VER
static double log2(double n) //log2() is not supported by MSVC
{
    // log(n)/log(2) is log2.
    return log( n ) / log( 2. );
}
#endif



NewExperimentSettingsDialog::NewExperimentSettingsDialog(short ModelType,IResponcesSource *pDataSource, int _interaction_level, int _paral, QWidget *parent):
    _modelType(ModelType),QDialog(parent),ui(new Ui::NewExperimentSettingsDialog),_interaction_level(_interaction_level),_paral(_paral)
{

    ui->setupUi(this);


    if (ModelType ==MATHMODEL)
    {
        ui->spinBox_interactionLevel->show();
        ui->label_8->show();
    }
    else
    {
        ui->spinBox_interactionLevel->hide();
        ui->label_8->hide();
    }

    isFormulaReferense=true;
    ui->rbtnGetYFromFunction->setChecked(true);
    ui->DrobRepl_horizontalSlider->setMinimum(2);
    ui->DrobRepl_horizontalSlider->setMaximum(3);
    ui->DrobRepl_horizontalSlider->setValue(1);



    if(pDataSource != NULL)
    {
        data_old = data = *pDataSource;
        def = true;
        loadSettings();
        ui->pushButton->setEnabled(true);
    }
    else
    {
        def = false;
        initValues();
    }
    init();
    connect(this,SIGNAL(close()),SLOT(on_NewExperimentSettingsDialog_destroyed()));

}

void NewExperimentSettingsDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->type() == QKeyEvent::KeyPress)
    {
        if(e->matches(QKeySequence::Copy))
        {
            clipCopy(ui->tableWidget_Znach);
        }
        else if (e->matches((QKeySequence::Paste)))
        {
            clipPaste(ui->tableWidget_Znach);
        }
    }
}

void NewExperimentSettingsDialog::loadSettings()
{
    ui->spinBox_interactionLevel->setValue(_interaction_level);
     ui->FactNum_spinBox->setValue(0);
    ui->FactNum_spinBox->setValue(data_old.FactNum);
    ui->DrobRepl_horizontalSlider->setValue(data_old.ActualFactNum);
    ui->spinBox_paral->setValue(_paral);
    data = data_old;

    QString DisplayFunc="";
    if (data.EvaluateFunction.count()>0)
    {
        DisplayFunc=data.EvaluateFunction.at(0);
    }
    if (DisplayFunc!="")
    {
        ui->tFormulaInput->setText(data.EvaluateFunction.at(0));
        if (data.EvaluateFunction.at(1).contains("/*degrees*/"))
        {
            ui->rbDegreeCorner->setChecked(true);
        }
    }
    else
    {
        ui->rbtnGetYFromNatur->setChecked(true);
    }

    if (ui->tableWidget_Znach->rowCount()!=data.FactNum)
    {
        ui->tableWidget_Znach->setRowCount(data.FactNum);
        ui->tableWidget_Znach->blockSignals(true);
        for (int i=0;i<ui->tableWidget_Znach->rowCount();i++)
        {
            for (int j=0;j<ui->tableWidget_Znach->columnCount();j++)
            {
                ui->tableWidget_Znach->setItem(i,j,new QTableWidgetItem(QString()));
            }
        }
        ui->tableWidget_Znach->blockSignals(false);
    }

    for (int i = 0; i < data.FactNum; i++)
    {
        ui->tableWidget_Znach->item(i,2)->setText(QString::number(data.centerFor(i)));

        double d;
        data.intervalFor(i, &d, &d);
        ui->tableWidget_Znach->item(i,3)->setText(QString::number(d));
        ui->tableWidget_Znach->item(i,1)->setText(data.Descriptions[i]);

        FactInfo tmp;
        tmp.Center=data.centerFor(i);
        tmp.Interval=d;
        tmp.FactorName=data.Descriptions[i];

        AllPrevFactors.append(tmp);
    }

   // ui->DrobRepl_horizontalSlider->setMinimum(data_old.MinFactNum);
    ui->DrobRepl_horizontalSlider->setMaximum(data_old.FactNum);
    ui->DrobRepl_horizontalSlider->setValue(data_old.ActualFactNum);

}

NewExperimentSettingsDialog::~NewExperimentSettingsDialog()
{
   if (ui)
    {
        delete ui;
    }
   close();
}

void NewExperimentSettingsDialog::clipCopy(QTableWidget *table)
{
    ClipCopyPaste ccp;
    ccp.clipCopy(table);
}

void NewExperimentSettingsDialog::clipPaste(QTableWidget *table)
{
    ClipCopyPaste ccp;
    ccp.clipPaste(table);
}


QString NewExperimentSettingsDialog::CheckExpressionPattern( QString pattern, QString text)
{
    if (text=="") return "";
    int pos=-1;
    QRegExp *exp = new QRegExp(pattern);
    QString matchedword="";
    exp->setPattern(pattern);
    pos=exp->indexIn(text);
    if (pos==0)
    {
        matchedword=exp->capturedTexts().first();
    }
    return matchedword;
}

QString NewExperimentSettingsDialog::ColorText(QString text, QString htmlFont)
{
    QString res="";
    res="<font color="+htmlFont+">"+text+"</font>";
    return res;
}

QVector<QString> NewExperimentSettingsDialog::GetExpressionFactorNames(QString text)
{
    QString FunctionReg = "^(sin\\(|asin\\(|cos\\(|acos\\(|tan\\(|atan\\(|ctan\\(|sqrt\\(|log\\(|exp\\(|pow\\()";
    QString Operators = "^(\\*|\\/|\\+|\\-|\\%)";
    QString Numbers = "([0-9]+|([0-9]+\\.[0-9]*))";
    QString Punctuation = "[\\,|\\s*|\\(|\\)|\\;|\\:|\\=]";

    QString  Varibles = "[_a-zA-Z]{1}[_0-9a-zA-Z]*";
    QString  XVaribles = "[xX]{1}[1-9]{1}[0-9]*";
    QString WrongVaribles="(([xX]{2,}[0-9a-zA-Z]*)|([xX]{1,}[0-9]{1,}[a-zA-Z]{1,}))";
    QString MatchedWord="";

    QVector<QString> FactorList;

    text=text.toLower();

    while (text!="")
    {
        if ((MatchedWord=CheckExpressionPattern(Numbers,text))!="")
        {
            text=text.replace(0,MatchedWord.length(),"");
        }
        else if ((MatchedWord=CheckExpressionPattern(Operators,text))!="")
        {
            text=text.replace(0,MatchedWord.length(),"");
        }
        else if ((MatchedWord=CheckExpressionPattern(FunctionReg,text))!="")
        {
            text=text.replace(0,MatchedWord.length(),"");
        }
        else if (!allowPsevdonamesForVaribles &&((MatchedWord=CheckExpressionPattern(WrongVaribles,text))!=""))
        {
            text=text.replace(0,MatchedWord.length(),MatchedWord.left(MatchedWord.length()-1));
        }
        else if ((MatchedWord=CheckExpressionPattern(Varibles,text))!="")
        {
            QString XMatchedWord="";
            if (!allowPsevdonamesForVaribles)
            {
                XMatchedWord=CheckExpressionPattern(XVaribles,text);
            }

            text=text.replace(0,MatchedWord.length(),"");
            if ((MatchedWord.toUpper() != "E") && (MatchedWord.toUpper() != "PI")&&(!FactorExists(FactorList,MatchedWord)))
            {
                if (!allowPsevdonamesForVaribles && XMatchedWord!="")
                    FactorList.append(XMatchedWord);
                else if (allowPsevdonamesForVaribles)
                    FactorList.append(MatchedWord);
            }
        }
        else if ((MatchedWord=CheckExpressionPattern(Punctuation,text))!="")
        {
            text=text.replace(0,MatchedWord.length(),"");
        }
        else if (text.length()>0)
        {
            text.replace(0,1,"");
        }
    }
    if (allowPsevdonamesForVaribles)
    {
        qSort(FactorList.begin(),FactorList.end());
    }
    else
    {
        QVector<int> Xnumbers;
        for (int i=0;i<FactorList.count();i++)
        {
            QString tmp= FactorList.at(i);
            tmp=tmp.toLower().remove("x");
            Xnumbers.append(tmp.toInt());
        }
        qSort(Xnumbers);
        FactorList.clear();
        for (int i=0;i<Xnumbers.count();i++)
        {
            FactorList.append("x"+QString::number(Xnumbers.at(i)));
        }
    }
    return FactorList;
}

QString NewExperimentSettingsDialog::ColorFormula(QString text)
{
    QString FunctionReg = "^(sin\\(|asin\\(|cos\\(|acos\\(|tan\\(|atan\\(|ctan\\(|sqrt\\(|log\\(|exp\\(|pow\\()";//(\\w*\\.{0,1}\\d*\\,{0,1})*\\)*";
    QString Operators = "^(\\*|\\/|\\+|\\-|\\%)";
    QString Numbers = "([0-9]+|([0-9]+\\.[0-9]*))";
    QString Punctuation = "[\\,|\\s*|\\(|\\)|\\;|\\:|\\=]";

    QString  Varibles = "[_a-zA-Z]{1}[_0-9a-zA-Z]*";
    QString  XVaribles = "[xX]{1}[1-9]{1}[0-9]*";
  /*  QString Varibles="";
    if (allowPsevdonamesForVaribles)
        Varibles = "[_a-zA-Z]{1}[_0-9a-zA-Z]*";
    else
        Varibles = "[xX]{1}[_0-9]*";
*/
    QString WrongVaribles="(([xX]{2,}[0-9a-zA-Z]*)|([xX]{1,}[0-9]{1,}[a-zA-Z]{1,}))";

    QString MatchedWord="";
    QString ColoredFormula="";

    QStringList FunctionList;

    haswronWaribles=false;
    int pos=0;
    wrongpos=-1;
    text= text.toLower();

    while (text!="")
    {
        if ((MatchedWord=CheckExpressionPattern(Numbers,text))!="")
        {
            text=text.replace(0,MatchedWord.length(),"");
            ColoredFormula+=ColorText(MatchedWord,"blue");
        }
        else if ((MatchedWord=CheckExpressionPattern(Operators,text))!="")
        {
            text=text.replace(0,MatchedWord.length(),"");
            ColoredFormula+=ColorText(MatchedWord,"black");
        }
        else if ((MatchedWord=CheckExpressionPattern(FunctionReg,text))!="")
        {
            text=text.replace(0,MatchedWord.length(),"");
            ColoredFormula+=ColorText(MatchedWord,"green");
        }
        else if (!allowPsevdonamesForVaribles &&((MatchedWord=CheckExpressionPattern(WrongVaribles,text))!=""))
        {
            text=text.replace(0,MatchedWord.length(),MatchedWord.left(MatchedWord.length()-1));
        }
        else if ((MatchedWord=CheckExpressionPattern(Varibles,text))!="")
        {
            QString XMatchedWord="";
            if (!allowPsevdonamesForVaribles)
            {
                XMatchedWord=CheckExpressionPattern(XVaribles,text);
            }

            text=text.replace(0,MatchedWord.length(),"");
            if ((MatchedWord.toUpper() != "E") & (MatchedWord.toUpper() != "PI"))
            {
                if (!allowPsevdonamesForVaribles && XMatchedWord!="" &&FactorExists(FactorNames,XMatchedWord))
                {
                    ColoredFormula+=ColorText(XMatchedWord,"red");
                }
                else if (!allowPsevdonamesForVaribles)
                {
                    ColoredFormula+=ColorText("<u>"+MatchedWord+"</u>","black");
                    if (wrongpos==-1)
                    {
                        wrongpos=pos+1;
                    }
                   haswronWaribles=true;
                }
                else if (FactorExists(FactorNames,MatchedWord))
                {
                    ColoredFormula+=ColorText(MatchedWord,"red");
                }
            }
            else
            {
                 ColoredFormula+=ColorText(MatchedWord.toUpper(),"green");
            }

        }
        else if ((MatchedWord=CheckExpressionPattern(Punctuation,text))!="")
        {
            text=text.replace(0,MatchedWord.length(),"");
            ColoredFormula+=ColorText(MatchedWord,"green");

        }
        else if (text.length()>0)
        {
            text.replace(0,1,"");
        }
        pos+=MatchedWord.length();
    }
    return ColoredFormula;
}


//---
NewExperimentSettingsDialog::FactInfo NewExperimentSettingsDialog::GetFactorInfo(QString FactorName)
{
    for (int i=0;i<AllPrevFactors.count();i++)
    {
        if (((FactInfo)(AllPrevFactors.at(i))).FactorName==FactorName)
        {
            return ((FactInfo)(AllPrevFactors.at(i)));
        }
    }
}


//---
void NewExperimentSettingsDialog::SetFactorInfo(QString FactorName,double Center,double Interval)
{
    for (int i=0;i<AllPrevFactors.count();i++)
    {
        if (((FactInfo)(AllPrevFactors.at(i))).FactorName==FactorName)
        {
            FactInfo tmp;
            tmp.FactorName=FactorName;
            tmp.Center=Center;
            tmp.Interval=Interval;
            AllPrevFactors[i]=tmp;
            return;
        }
    }
}


NewExperimentSettingsDialog::FactInfo NewExperimentSettingsDialog::GetFactorInfo(int FactorNumber)
{
    return ((FactInfo)(AllPrevFactors.at(FactorNumber)));
}

void NewExperimentSettingsDialog::SetFactorInfo(int FuctorNumber, QString FactorName, double Center, double Interval)
{
    FactInfo tmp;
    tmp.FactorName=FactorName;
    tmp.Center=Center;
    tmp.Interval=Interval;
    AllPrevFactors[FuctorNumber]=tmp;
    return;
}


bool NewExperimentSettingsDialog::FactorExists(QVector<QString> FactorList,QString FactorName)
{
    for (int i = 0; i < FactorList.count(); i++)
    {
        if (FactorList.at(i) == FactorName) return true;
    }

    return false;
}

void NewExperimentSettingsDialog::on_FactNum_spinBox_valueChanged(int arg1)
{
    int PrevFactCount=AllPrevFactors.count();
    int CurFactCount=arg1;

    if (CurFactCount>PrevFactCount)
    {
        for (int i=PrevFactCount;i<CurFactCount;i++)
        {
            FactInfo tmp;
            tmp.FactorName="";
            tmp.Center=0;
            tmp.Interval=1.0;
            AllPrevFactors.append(tmp);
        }
    }

    data.FactNum = arg1;
    data.PFEnum = 1 << arg1;
    ui->PFEnum_labelPFE->setText(QString::number(data.PFEnum));
    //ограничение уровня взаимодействия числом факторов
    int iteractionLevel=1;
    if (data.FactNum>1)
    {
        iteractionLevel=data.FactNum-1;
    }

    ui->spinBox_interactionLevel->setMinimum(1);
    ui->spinBox_interactionLevel->setMaximum(iteractionLevel);

    //слайдер дробности реплики ограничивается допустимыми макс значениями
    ui->DrobRepl_horizontalSlider->setMaximum(data.FactNum);
    ui->DrobRepl_horizontalSlider->setValue(data.FactNum);
    // при изменении числа факторов, сразу вычисляется их минимально необходимое количество
    calcMinFactNum();
    updateTables();
}

void NewExperimentSettingsDialog::on_DrobRepl_horizontalSlider_valueChanged(int value)
{
    data.DrobRepl = (double)value / data.FactNum;
    double drob = pow(2., data.FactNum) / pow(2, (double)value);
    ui->label_drob->setText(QString::number(drob));
    data.ActualFactNum = value;
    ui->label_FactBase->setText(QString::number(data.ActualFactNum));
    ui->PFEnum_labelPFE->setText(QString::number(pow(2.,value)));
    updateTables();
}


void NewExperimentSettingsDialog::on_pushButton_clicked()
{

    if (this->isFormulaReferense)
    {
        FormulaCheck();
    }
    else
    {
        if (ui->FactNum_spinBox->value()<2)
        {
            QMessageBox::about(this,"Ошибка",QString("<p><h3>Недостаточно факторов</h3></p>"));
            return;
        }

        bool res=saveFactValues();
        if (res)
        {
            savedata=true;
            close();
        }
    }
}



void NewExperimentSettingsDialog::on_pushButton_2_clicked()
{
    savedata=false;
    close();
}

void NewExperimentSettingsDialog::on_tFormulaInput_textChanged()
{
    int startpos=ui->tFormulaInput->textCursor().position();
    QString formula=ui->tFormulaInput->toPlainText().toUtf8();

    FactorNames= GetExpressionFactorNames(formula);

    ui->tFormulaInput->blockSignals(true);
        formula=ui->tFormulaInput->toPlainText();
        ui->tFormulaInput->clear();
        ui->tFormulaInput->insertHtml(ColorFormula(formula));
    ui->tFormulaInput->blockSignals(false);

    this->EvaluateString=ui->tFormulaInput->toPlainText();

    //move cursor
    int endpos= ui->tFormulaInput->textCursor().position();
    for (int i = 0; i <endpos-startpos; i++)
        ui->tFormulaInput->moveCursor(QTextCursor::Left, QTextCursor::MoveAnchor);
}

void NewExperimentSettingsDialog::on_tableWidget_Znach_cellChanged(int row, int column)
{
    // NEW NEW 24.01 NEW NEW NEW //
    ui->tableWidget_Znach->resizeColumnsToContents();
    ui->tableWidget_Znach->resizeRowsToContents();

    // NEW NEW 24.01 NEW NEW NEW //
    if(auto_check)
        switch(column)
        {
        case FACT_NAME_COLUMN: // псевдоним
        {
            QString FactorName= ui->tableWidget_Znach->item(row,1)->text();
            SetFactorInfo(row,FactorName,0,1);
            break;
        }
        case FACT_CENTER_COLUMN: // центр фактора
            onCenterChanged(row, column);
            break;

        case FACT_DIAPAZON_COLUMN: // диапазон выррирования
        {
            ui->tableWidget_Znach->blockSignals(true);
            double diapazon=ui->tableWidget_Znach->item(row,FACT_DIAPAZON_COLUMN)->text().toDouble();
            ui->tableWidget_Znach->item(row,FACT_INTERVAL_COLUMN)->setText(QString::number(diapazon/2.));
            ui->tableWidget_Znach->blockSignals(false);
            onCenterChanged(row, column - 1);
            break;
        }

        case FACT_MIN_COLUMN: // минимальное значение
            onMinMaxChanged(row, column + 1);
            break;

        case FACT_MAX_COLUMN: // максимальное
            onMinMaxChanged(row, column);
            break;
        case FACT_INTERVAL_COLUMN:
        {
            ui->tableWidget_Znach->blockSignals(true);
            double interval=ui->tableWidget_Znach->item(row,FACT_INTERVAL_COLUMN)->text().toDouble();
            ui->tableWidget_Znach->item(row,FACT_DIAPAZON_COLUMN)->setText(QString::number(interval*2.));
            ui->tableWidget_Znach->blockSignals(false);
            onCenterChanged(row, FACT_DIAPAZON_COLUMN - 1);

            break;
        }
        }
}

void NewExperimentSettingsDialog::on_spinBox_paral_valueChanged(int arg1)
{
    data.parall = (double)arg1;
}

IResponcesSource* NewExperimentSettingsDialog::getDataSource()
{
    if (!canceled)
    {
        saveFactValues();
        return new ResponcesSourseFunction(data);
    }
    else
    {
        if (def)
            return new ResponcesSourseFunction(data_old);
        else
            return NULL;
    }
}

void NewExperimentSettingsDialog::init()
{
    canceled = false;
    savedata=false;
    this->setWindowFlags(Qt::Window |
                         Qt::CustomizeWindowHint |
                         Qt::WindowTitleHint |
                         Qt::WindowMinimizeButtonHint |
                         Qt::WindowMaximizeButtonHint |
                         Qt::WindowCloseButtonHint |
                         Qt::WindowSystemMenuHint
                         );
    QStringList labels;
    labels.append(QString::fromUtf8("№\nфактора"));
    labels.append(QString::fromUtf8("Описание"));
    labels.append(QString::fromUtf8("Значения\nцентра\nплана"));
    labels.append(QString::fromUtf8("Диапазон\nварьирования"));
    labels.append(QString::fromUtf8("Минимальное\nзначение"));
    labels.append(QString::fromUtf8("Максимальное\nзначение"));
    labels.append(QString::fromUtf8("Интервал\nварьирования"));


    ui->tableWidget_Znach->setHorizontalHeaderLabels(labels);
    ui->tableWidget_Znach->resizeColumnsToContents();
}

void NewExperimentSettingsDialog::initValues()
{
    ui->DrobRepl_horizontalSlider->setMinimum(2);
    ui->DrobRepl_horizontalSlider->setMaximum(3);
    ui->FactNum_spinBox->setValue(1);
    ui->FactNum_spinBox->setValue(0);
    ui->DrobRepl_horizontalSlider->setValue(2);
}

bool NewExperimentSettingsDialog::askNewSettings()
{
    this->exec();

    return savedata;

    if (canceled)
        return false;
    else
        return true;
}

bool NewExperimentSettingsDialog::saveFactValues()
{

    canceled=false;

    bool r = true;

    data.EvaluateFunction.clear();


    // для формулы нет паралельных опытов
    if (ui->rbtnGetYFromFunction->isChecked())
    {
        data.parall =1;
        QString evalFunction=EvaluateString;
        if (ui->rbDegreeCorner->isChecked())
        {

            evalFunction=data.CheckFormulaForTrinometricFunctions(EvaluateString);
            evalFunction="/*degrees*/"+evalFunction;
        }
        data.SetEvaluateFunction(EvaluateString,evalFunction);
    }
    else
    {
        int paral=ui->spinBox_paral->value();
        if (paral<0) paral=0;
        data.parall=(double) paral;
        data.SetEvaluateFunction("","");
    }

    if (ui->FactNum_spinBox->value()<1)
        return false;

    data.FactValues.clear();
    data.FactDivergences.clear();

    data.DrobRepl =(double)ui->DrobRepl_horizontalSlider->value();


    //данные из таблицы заносятся в соответствующие вектора
    for (int i = 0; i < ui->tableWidget_Znach->rowCount(); i++)
    {
        if (!(ui->tableWidget_Znach->item(i, 2)->text().isEmpty()))
            data.FactValues.push_back(
                ui->tableWidget_Znach->item(i, 2)->text().toDouble() );
        else
        {
            QMessageBox msg;
            msg.setText("Fact values are not entered completely");
            msg.exec();
            r = false;
            break;
        }

        if (!(ui->tableWidget_Znach->item(i, 3)->text().isEmpty()))
            data.FactDivergences.push_back(
                ui->tableWidget_Znach->item(i, 3)->text().toDouble() );
        else
        {
            QMessageBox msg;
            msg.setText("Fact divergences are not entered completely");
            msg.exec();
            r = false;
            break;
        }
    }

    data.Descriptions.clear();

    for (int i = 0; i < ui->tableWidget_Znach->rowCount(); i++)
    {
        data.Descriptions.append(ui->tableWidget_Znach->item(i, 1)->text());
    }
    savedata=true;
    return r;
}


void NewExperimentSettingsDialog::updateTables()
{
    auto_check = false;
    int oldRows;
    oldRows = ui->tableWidget_Znach->rowCount();
    ui->tableWidget_Znach->setRowCount(data.FactNum);

    if (oldRows < data.ActualFactNum)
    {
        double center=0;
        double interval=1.;
        QString name="";
        for (int i = oldRows; i < ui->tableWidget_Znach->rowCount(); i++)
        {
            if (AllPrevFactors.count()!=0)
            {
               name= GetFactorInfo(i).FactorName;
               center=GetFactorInfo(i).Center;
               interval=GetFactorInfo(i).Interval;
            }
            ui->tableWidget_Znach->setItem(i, FACT_CENTER_COLUMN, new QTableWidgetItem(QString::number(center)));
            ui->tableWidget_Znach->setItem(i, FACT_DIAPAZON_COLUMN, new QTableWidgetItem(QString::number(interval)));
            ui->tableWidget_Znach->setItem(i, FACT_INTERVAL_COLUMN, new QTableWidgetItem(QString::number(interval/2.)));
            ui->tableWidget_Znach->setItem(i, FACT_MIN_COLUMN, new QTableWidgetItem(QString::number(center-interval/2.)));
            ui->tableWidget_Znach->setItem(i, FACT_MAX_COLUMN, new QTableWidgetItem(QString::number(center+interval/2.)));
             ui->tableWidget_Znach->setItem(i, FACT_NAME_COLUMN, new QTableWidgetItem(name));

            ui->tableWidget_Znach->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
            ui->tableWidget_Znach->item(i, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        }
    }

    auto_check = true;
}

void NewExperimentSettingsDialog::updateTables(QString factorname)
{
    auto_check = false;
    int oldRows;
    oldRows = ui->tableWidget_Znach->rowCount();
    ui->tableWidget_Znach->setRowCount(data.FactNum);


    if (oldRows < data.ActualFactNum)
    {
        for (int i = oldRows; i < ui->tableWidget_Znach->rowCount(); i++)
        {
            ui->tableWidget_Znach->setItem(i, 2, new QTableWidgetItem("0.0"));
            ui->tableWidget_Znach->setItem(i, 3, new QTableWidgetItem("1.0"));
            ui->tableWidget_Znach->setItem(i, 4, new QTableWidgetItem("-0.5"));
            ui->tableWidget_Znach->setItem(i, 5, new QTableWidgetItem("0.5"));
            ui->tableWidget_Znach->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
            ui->tableWidget_Znach->setItem(i, 0, new QTableWidgetItem(factorname));
            ui->tableWidget_Znach->item(i, 0)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            ui->tableWidget_Znach->setItem(i, 1, new QTableWidgetItem(""));
        }
    }

    auto_check = true;
}

void NewExperimentSettingsDialog::calcMinFactNum()
{
    data.MinFactNum = -1;

    for (int i = 2; i <= data.FactNum; i++)
    {
        if (pow(2., i) > data.FactNum)
        {
            data.MinFactNum = i;
            break;
        }
    }

    if (data.MinFactNum == -1)
        data.MinFactNum = data.FactNum;

    data.MaxDrobRepl =
        pow(2., data.MinFactNum) / pow(2., data.FactNum);
    data.ActualFactNum = data.MinFactNum;
    ui->label_FactBase->setText(QString::number(data.ActualFactNum));

    //слайдер дробности реплики ограничивается допустимыми мин значениями
    ui->DrobRepl_horizontalSlider->setMinimum(data.MinFactNum);
    ui->DrobRepl_horizontalSlider->setValue(data.MinFactNum);
}

void NewExperimentSettingsDialog::onMinMaxChanged(int row, int col)
{
    double interval, max, min, center;
    max = ui->tableWidget_Znach->item(row, col)->text().toDouble();
    min = ui->tableWidget_Znach->item(row, col - 1)->text().toDouble();
    interval = max - min;
    center = max - interval / 2.;

    auto_check = false;
    ui->tableWidget_Znach->item(row, 2)->setText(QString::number(center));
    ui->tableWidget_Znach->item(row, FACT_DIAPAZON_COLUMN)->setText(QString::number(interval));
    ui->tableWidget_Znach->item(row, FACT_INTERVAL_COLUMN)->setText(QString::number(interval/2.));
    auto_check = true;

    QString FactorName= ui->tableWidget_Znach->item(row,1)->text();
    SetFactorInfo(row,FactorName,center,interval);
}

void NewExperimentSettingsDialog::onCenterChanged(int row, int col)
{
    //хоть называется и onCenter, на самом деле отвечает и за центр и за диапазон
    double interval, max, min, center;
    interval = ui->tableWidget_Znach->item(row, col + 1)->text().toDouble();
    center = ui->tableWidget_Znach->item(row, col)->text().toDouble();
    max = center + interval / 2;
    min = center - interval / 2;

    auto_check = false;
    ui->tableWidget_Znach->item(row, 4)->setText(QString::number(min));
    ui->tableWidget_Znach->item(row, 5)->setText(QString::number(max));
    auto_check = true;

    QString FactorName= ui->tableWidget_Znach->item(row,1)->text();
    SetFactorInfo(row,FactorName,center,interval);

    //SetFactorInfo(FactorName,center,interval);
}

int NewExperimentSettingsDialog::getInteractionLevel()
{
    return ui->spinBox_interactionLevel->value();
}

int NewExperimentSettingsDialog::GetModelType()
{
    // есть функция
    if (EvaluateString!="") return 0;
    return 1;
}

void NewExperimentSettingsDialog::FormulaCheck()
{
    if (ui->tFormulaInput->toPlainText()=="")
    {
        QMessageBox::about(this,"Ошибка", QString("<p><h3>Отсутствует формула</h3></p>"));
        return;
    }

    if (haswronWaribles)
    {

        QMessageBox::about(this,"Ошибка", QString("<p><h3>Введёная формула содержит неверные имена переменных</h3></p>")+
                           QString("<p><b> Описание ошибки: <i>Проверьте синтаксис или воспользуйтесь справкой</i></b></p>")+
                           QString("<p><b>Строка: 1")+
                           QString("; Столбец: ")+QString::number(wrongpos)+QString("</p>") +
                           QString(this->EvaluateString.left(wrongpos-1)+"<font color=red>"+this->EvaluateString.at(wrongpos-1)+"</font>"+this->EvaluateString.right(this->EvaluateString.length()-wrongpos)));
        return;
    }

    if (!allowPsevdonamesForVaribles)
    {
        QVector<int> Xnumbers;
        for (int i=0;i<FactorNames.count();i++)
        {
            QString tmp= FactorNames.at(i);
            tmp=tmp.toLower().remove("x");
            Xnumbers.append(tmp.toInt());
        }
        qSort(Xnumbers);

        if (Xnumbers.at(0)!=1)
        {
            QMessageBox::about(this,"Ошибка",QString("<p><h3>Введёная формула содержит не все переменные</h3></p>")+
                                       QString("<p><b> Описание ошибки: <i>Отсутствует переменная <font color=red>x1</font></i></b></p>"));
            return;
        }

        for (int i=0;i<Xnumbers.count()-1;i++)
        {
            if (Xnumbers.at(i+1)>data.FactNum)
            {
                QMessageBox::about(this,"Ошибка",QString("<p><h3>Введённая формула содержит переменные, отсутствующие в настройках плана эксперимента</h3></p>")+
                                           QString("<p><b> Описание ошибки: <i>Отсутствует переменная <font color=red>x"+QString::number(Xnumbers.at(i)+1)+"</font></i></b></p>"));
                return;
            }

            if (Xnumbers.at(i)!=Xnumbers.at(i+1)-1)
            {
                QMessageBox::about(this,"Ошибка",QString("<p><h3>Введёная формула содержит не все переменные</h3></p>")+
                                           QString("<p><b> Описание ошибки: <i>Отсутствует переменная <font color=red>x"+QString::number(Xnumbers.at(i)+1)+"</font></i></b></p>"));
                return;
            }

        }//
        int FactsInFormula=GetExpressionFactorNames(this->EvaluateString).count();
                if (data.FactNum>FactsInFormula)
                {
                    QMessageBox::about(this,"Ошибка",QString("<p><h3>Не все переменные присутствуют в функциональной зависимости</h3></p>"));
                    return;
                }
    }

    QScriptEngine RunScript;

    QScriptSyntaxCheckResult ErrVal=RunScript.checkSyntax(this->EvaluateString);
// Проверка синтаксиса введёной функции
    if (ErrVal.state()== QScriptSyntaxCheckResult::Valid)
    {   // syntax correct
        // Проверка количества введёных факторов
        if (this->data.FactNum<2)
        {
            QMessageBox::about(this,"Ошибка",QString("<p><h3>Введёная формула содержит менее двух переменных</h3></p>")+
                               QString("<p><b> Описание ошибки: <i>Данная программа разработана для работы с формулами, содержащими более одной переменной</i></b></p>"));
        }
        else if (!RunScript.canEvaluate(this->EvaluateString))
        {
            QMessageBox::about(this,"Ошибка",QString("<p><h3>Невозможно вычислить введёную формулу</h3></p>")+
                               QString("<p><b> Описание ошибки: <i>Проверьте синтаксис или воспользуйтесь справкой</i></b></p>"));
        }
        else if (saveFactValues())
        {
            close();
        }

    }
    else
    {
        // syntax not correct
        QString tmp_formula = this->EvaluateString;
        int ErrColum=0,ErrLine=0;
        if (ErrVal.errorColumnNumber()<1)
            ErrColum=1;
        else ErrColum= ErrVal.errorColumnNumber();

        if (ErrVal.errorLineNumber()<1)
            ErrLine=0;
        else
            ErrLine=ErrVal.errorLineNumber();

        QMessageBox::about(this,"Ошибка", QString("<p><h3>Введёная формула содержит ошибки</h3></p>")+
                           QString("<p><b> Описание ошибки: <i>"+tr(ErrVal.errorMessage().toLatin1())+"</i></b></p>")+
                           QString("<p><b>Строка: ")+QString::number(ErrLine)+
                           QString("; Столбец: ")+QString::number(ErrColum)+QString("</p>")+
                           QString(tmp_formula.left(ErrColum-1)+"<font color=red>"+tmp_formula.at(ErrColum-1)+"</font>"+tmp_formula.right(tmp_formula.length()-ErrColum)));
    }

}




void NewExperimentSettingsDialog::on_rbtnGetYFromFunction_toggled(bool checked)
{
    if (checked)
    {
    isFormulaReferense=true;
    ui->tFormulaInput->setText("");
    ui->rbRadianCorner->setChecked(true);
    ui->grpBoxFormulaParams->show();

    ui->label_7->hide();
    ui->spinBox_paral->hide();
    }
}


void NewExperimentSettingsDialog::on_rbtnGetYFromNatur_toggled(bool checked)
{
    if (checked)
    {
    isFormulaReferense=false;
    ui->grpBoxFormulaParams->hide();

    ui->label_7->show();
    ui->spinBox_paral->show();
    }
}

void NewExperimentSettingsDialog::on_saveFormula_clicked()
{
    QString message=QString::fromUtf8("Во время сохранения файла возникли ошибки,проверьте введённые параметры и повторите попытку");
    QString FileName="";

 /*
    if (ui->tFormulaInput->toPlainText()=="")
    {
        message =QString::fromUtf8("Отсутствует формула");
        QMessageBox msg;
        msg.setText(message);
        msg.setWindowTitle(" ");
        msg.setSizeGripEnabled(true);
        msg.exec();

    }
    else
    */

    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
    QString path="";
    path=settings->value("settings/examle_XML").toString();


        FileName=QFileDialog::getSaveFileName(this, ("Сохранить"), path, ("Table (*.xml);;All Files(*)"));
    if (FileName!="")// && ui->tFormulaInput->toPlainText()!="")
    {

        if (saveFactValues())//&& this->ui->tFormulaInput->toPlainText()!="")
        {

            if (this->data.SaveFormula(FileName))
            {
                message =QString::fromUtf8("Файл успешно сохранён");
            }
        }
        else
        {
            message =QString::fromUtf8("Отстутствуют данные для сохранения");
        }
    }
    if (FileName!="")
    {        
        QMessageBox msg;
        msg.setText(message);
        msg.setWindowTitle(" ");
        msg.setSizeGripEnabled(true);
        msg.exec();
    }
}

void NewExperimentSettingsDialog::on_loadFormula_clicked()
{
    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
    QString path="";
    path=settings->value("settings/examle_XML").toString();


    QString FileName=QFileDialog::getOpenFileName(this, ("Сохранить"), path, ("Table (*.xml);;All Files(*)"));
    if (FileName!="")
    {
        if(this->data.LoadFormula(FileName))
        {

            ui->FactNum_spinBox->setValue(data.FactNum);

            ui->tFormulaInput->setText(data.EvaluateFunction.at(0));
            if (data.EvaluateFunction.at(1).contains("/*degrees*/"))
            {
                ui->rbDegreeCorner->setChecked(true);
            }

            if (ui->tableWidget_Znach->rowCount()!=data.FactNum)
            {
                ui->tableWidget_Znach->setRowCount(data.FactNum);
                    ui->tableWidget_Znach->blockSignals(true);
                for (int i=0;i<ui->tableWidget_Znach->rowCount();i++)
                {
                    for (int j=0;j<ui->tableWidget_Znach->columnCount();j++)
                    {
                        ui->tableWidget_Znach->setItem(i,j,new QTableWidgetItem(QString()));
                    }
                }
                ui->tableWidget_Znach->blockSignals(false);

            }

            for (int i = 0; i < data.FactNum; i++)
            {
                if (data.FactValues.size()>0)
                {
                    ui->tableWidget_Znach->item(i,2)->setText(QString::number(data.FactValues.at(i)));
                }

                if (data.FactDivergences.size()>0)
                {
                    ui->tableWidget_Znach->item(i,3)->setText(QString::number(data.FactDivergences.at(i)));
                }
                if (data.Descriptions.count()>0)
                {
                    ui->tableWidget_Znach->item(i,1)->setText(data.Descriptions[i]);
                }
                else
                {
                    ui->tableWidget_Znach->item(i,1)->setText(" ");
                }
            }




        }
        else
        {
            QMessageBox msg;
            msg.setText(QString::fromUtf8("Невозможно открыть указанный файл"));
            msg.setWindowTitle(" ");
            msg.setSizeGripEnabled(true);
            msg.exec();
        }
    }
}




void NewExperimentSettingsDialog::on_btnShowHelp_clicked()
{
    formula_setting_help_dialog * help = new formula_setting_help_dialog();
    help->show();
}

int NewExperimentSettingsDialog::getParal()
{
    return ui->spinBox_paral->value();
}

void NewExperimentSettingsDialog::on_pushButtonExportToCSVParams_clicked()
{
    if (ui->FactNum_spinBox->value()<2)
    {
        QMessageBox::about(this,"Ошибка",QString("<p><h3>Недостаточно факторов</h3></p>"));
        return;
    }
    else
    {
        QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
        QString path="";
        path=settings->value("settings/examle_CSV").toString();

        QString fileName = QFileDialog::getSaveFileName(this, ("Сохранить"), path /*"new_experiment.csv"*/, ("CSV(*.csv);;All Files(*)"));
        this->saveToCSV(fileName);


        QMessageBox msg;
        msg.setText(QString::fromUtf8("Данные успешно сохранены"));
        msg.setWindowTitle(" ");
        msg.setSizeGripEnabled(true);
        msg.exec();
    }
}
bool NewExperimentSettingsDialog::saveToCSV(const QString &fileName)
{
    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly))
        return false;

    QTextStream dstStream(&file);
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    if (codec!= NULL)
    {
        dstStream.setCodec(codec);
    }
    else
    {
        assert(1);
    }

    if (ui->rbtnGetYFromFunction->isChecked())
    {
        dstStream << QString::fromUtf8("Функция:") << ";" << ui->tFormulaInput->toPlainText() << "\r\n";
        if (ui->rbRadianCorner)
        {
            dstStream << QString::fromUtf8("Мера угла:") << ";" << "radian" << "\r\n";
        }
        else
        {
            dstStream << QString::fromUtf8("Мера угла:") << ";" << "degrees" << "\r\n";
        }
    }

    dstStream << QString::fromUtf8("ТАБЛИЦА ФАКТОРОВ;\r\n");
    dstStream << QString::fromUtf8("Число факторов:") << ";" << ui->FactNum_spinBox->text() << "\r\n";
    dstStream << QString::fromUtf8("Число опытов ПФЭ:") << ';' << ui->PFEnum_labelPFE->text() << ';' << "\r\n";
    dstStream << QString::fromUtf8("Дробность реплики:") << ";" << ui->label_drob->text() << ";\r\n";
    dstStream << QString::fromUtf8("Число базовых факторов:") << ";" << ui->label_FactBase->text() << ";\r\n";
    dstStream << QString::fromUtf8("Центры плана:") << ';' << "\r\n";
    for(size_t i = 0; i < ui->tableWidget_Znach->rowCount(); i++)
    {
        dstStream << ';' << ExperimentTable::doubleWithComma( ui->tableWidget_Znach->item(i, 2)->text().toDouble()) << ';';
        dstStream << "\r\n";
    }
    dstStream << QString::fromUtf8("Интервал варьирования:") << ';' << "\r\n";
    for(size_t i = 0; i < ui->tableWidget_Znach->rowCount(); i++)
    {
        dstStream << ';' << ExperimentTable::doubleWithComma(ui->tableWidget_Znach->item(i, 6)->text().toDouble()) << ';';
        dstStream << "\r\n";
    }
    dstStream << QString::fromUtf8("Размер описаний:") << ';';
    dstStream << ui->tableWidget_Znach->rowCount() << ';' << "\r\n";

    dstStream << QString::fromUtf8("Описания:") << ';' << "\r\n";
    for(size_t i = 0; i < ui->tableWidget_Znach->rowCount(); i++)
    {
        dstStream << ';' << ui->tableWidget_Znach->item(i, 1)->text() << ';';
        dstStream << "\r\n";
    }
    dstStream << QString::fromUtf8("Уровень взаимодействия:") << ";" << ui->spinBox_interactionLevel->text() << ";\r\n";
    if (MATHMODEL == _modelType)
    {
        dstStream << QString::fromUtf8("Тип модели:") << ";" << "MATHMODEL" << ";\r\n";
    }
    else
    {
        dstStream << QString::fromUtf8("Тип модели:") << ";" << "OPTIMUM" << ";\r\n";
        dstStream << QString::fromUtf8("Поиск точек перегиба") << ";\r\n";
        dstStream << QString::fromUtf8("По максимуму:") << ";" << "1" << ";\r\n";
        dstStream << QString::fromUtf8("Величина шага:") << ";" << "0,5" << ";\r\n";
        dstStream << QString::fromUtf8("Количество шагов:") << ";" << "7" << ";\r\n";
        dstStream << QString::fromUtf8("Допустимое отклонение:") << ";" << "30" << ";\r\n";
    }
    dstStream << QString::fromUtf8("Количество параллельных опытов:") << ";" << ui->spinBox_paral->text() << "\r\n";

}
