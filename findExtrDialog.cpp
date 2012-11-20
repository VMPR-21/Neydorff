#include "findExtrDialog.h"
#include "ui_findExtrDialog.h"
#include "IExperimentTable.h"
#include "IFactorTable.h"
#include "IRegressionCoefficientTable.h"
#include "bCoeff.h"
#include "ResponcesSourseFunction.h"

#include <math.h>
#include "Experementator.h"
#include "QModelIndex"
#include "mainwindow.h"
#include <QtGui>

findExtrDialog::findExtrDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::findExtrDialog)
{

    ui->setupUi(this);
}

findExtrDialog::~findExtrDialog()
{
    delete ui;
}

void findExtrDialog::startfindExtr(IExperimentTable &table, IResponcesSource &src)
{
    ui->pushButton_CCP->setDisabled(true);
    ui->pushButton_Experiment->setDisabled(true);
    this->index1 = -1;
    this->index2 = -1;
    initial = false;
    this->table = &table;
    this->src = &src;
    xP = 0;  //шаг, начиная с какого перевалило за 1
    xP_find = false;
    ccp = false;
    exper = false;

    h_edit = false;

    //QTableWidgetItem* ptwi = 0;

    isMax = table.getIsMax();
    if (isMax) {
        ui->radioButton->setChecked(true);
    } else {
        ui->radioButton_2->setChecked(true);
    }

    h = table.getStrideParameter();
    ui->doubleSpinBox->setValue(h);

    ch = table.getNumberStride();
    ui->spinBox_3->setValue(ch);

    dev = table.getInterestAllowedDeviation();
    ui->spinBox_2->setValue(dev);





    std::vector<bCoeff> bCoefs = table.b().coeffs();

    ui->tableWidget->setColumnCount(table.b().count());
    ui->tableWidget->setRowCount(1);
    ui->tableWidget->verticalHeader()->hide();

    QStringList lst;
    for(int i = 0; i < table.b().count(); i++)
    {
        lst << "β" + QString::number(i);
    }

    for(int i = 0; i < lst.size(); i++)
    {
        ui->tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(lst[i]));
    }

    for(int i = 0; i < table.b().count(); i++)
    {
        ui->tableWidget->setColumnWidth(i,65);
        //double xi = bCoefs[i].is_significant() ? bCoefs[i].natural_value() * (isMax ? 1.0 : -1.0) : 0.0;
        double xi =  bCoefs[i].natural_value() * (isMax ? 1.0 : -1.0);

        QTableWidgetItem *item = new QTableWidgetItem(QString::number(xi));
        item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
        ui->tableWidget->setItem(0, i, item);
        if (bCoefs[i].is_significant())
            ui->tableWidget->item(0, i)->setBackground(QColor(0, 255, 0, 127));
            //ui->tableWidget->item(0,i)->setBackground(Qt::green);
    }


    int iiiiiii = table.x().count()  ;
    int iiiiiii2 = table.b().count();  ;

    bCoefs.clear();
    bCoefs = table.b().coeffs();

    grad.clear();

    //перепишем b натуральные в градиент, при этом если незначим, то 0
    for(int i = 1; i < table.b().count(); i++)
    {
        //double xi = bCoefs[i].natural_value() * (isMax ? 1.0 : -1.0);
        double xi = bCoefs[i].is_significant() ? bCoefs[i].natural_value() * (isMax ? 1.0 : -1.0) : 0.0;
        grad.push_back(xi);
    }

    ui->tableWidget_2->setColumnCount(table.b().count()+7);
    ui->tableWidget_2->setRowCount(ch);
    ui->tableWidget_2->verticalHeader()->hide();


    lst.clear();
    lst << "№";

    int countGrad = grad.size();

    for(int i = 1; i <= grad.size(); i++)
    {
        lst << "x" + QString::number(i);
    }
    lst << "Ygrad";
    lst << "Yreal";
    lst << "Ygrad-Yreal";
    lst << "Знак";

    lst << "Приращение\n по шагу";
    lst << "Ускорение или\n замедление\n движения к extr";

    lst << "% отклонения\n Ygrad от Yreal";

    int countlst = lst.size();

    for(int i = 0; i < lst.size(); i++)
    {
        ui->tableWidget_2->setColumnWidth(i,50);
        ui->tableWidget_2->setHorizontalHeaderItem(i, new QTableWidgetItem(lst[i]));
    }

    ui->tableWidget_2->setColumnWidth(0,20);

    ui->tableWidget_2->setColumnWidth(lst.size()-1,86);
    ui->tableWidget_2->setColumnWidth(lst.size()-2,90);
    ui->tableWidget_2->setColumnWidth(lst.size()-3,80);

    ui->tableWidget_2->setColumnWidth(lst.size()-4,42);
    ui->tableWidget_2->setColumnWidth(lst.size()-5,70);
    ui->tableWidget_2->setColumnWidth(lst.size()-6,60);
    ui->tableWidget_2->setColumnWidth(lst.size()-7,60);



    grad.clear();

    //перепишем b натуральные в градиент, при этом если незначим, то 0
    for(int i = 1; i < table.b().count(); i++)
    {
        double xi = bCoefs[i].is_significant() ? bCoefs[i].natural_value() * (isMax ? 1.0 : -1.0) : 0.0;
        grad.push_back(xi);
    }


    //Поиск max коэффициента в градиенте
    double max = fabs(grad[0]);

    for(unsigned int i = 0; i < grad.size(); i++)
        if (max < fabs(grad[i])) max = fabs(grad[i]);



    std::vector<double> vecH;    //вектор шагов

    for(unsigned int i = 0; i < grad.size(); i++)
    {
        vecH.push_back(h * grad[i] / max);
    }


    std::vector<double> vecHbuf;    //вектор шагов

    for(unsigned int i = 0; i < grad.size(); i++)
    {
        vecHbuf.push_back(0);
    }

    //расчет xs и функции yt
    for (int i = 0; i < ch; i++)
    {
        if(xP_find == false)
        {
            xP = xP + h;

            if(xP > 1)
            {
                xP_find = true;
                xP = i;
            }
        }

        vecHbuf.clear();

        yt = bCoefs[0].natural_value();

        for (unsigned int j = 0; j < grad.size(); j++)
        {
            double buf = 0;
            buf = vecH[j] * (i + 1);
            vecHbuf.push_back(buf);
            yt = yt + grad[j] * buf;
        }

        P.xs.clear();
        P.xs = vecHbuf;

        for(size_t j = 0; j < P.xs.size(); j++)
        {
            if (bCoefs[j+1].is_significant())
            {
                if(bCoefs[j+1].index().size() == 1)
                {
                   double xdelta = table.x().xDelta(j);
                   double xcenter = table.x().xCenter(j);
                   int ccc =0;
                   P.xs[j] = table.x().xCenter(j) + table.x().xDelta(j) * P.xs[j];
                   int ccc2 =0;
                }
                else
                {
                    double buf = 0;
                    for(size_t k = 0; k < bCoefs[j+1].index().size(); k++)
                    {
                        buf += table.x().xCenter(bCoefs[j+1].index()[k]) + table.x().xDelta(bCoefs[j+1].index()[k]) * P.xs[j];
                    }
                    P.xs[j] = buf;
                }
            }
            else P.xs[j] = 0;
        }
/*
        std::vector<double> xxx;
        xxx.push_back(5.5);
        xxx.push_back(5.5);
        xxx.push_back(5.5);
        std::vector<double> yyy = src.getYdata2(xxx);
*/

        std::vector<double> yy = src.getYdata2(P.xs);
        int xsize = yy.size();
        if (yy.size()!=0)
            P.yp = yy[0];
        else
            P.yp = table.evalFuncValue(P.xs);

        P.yt = table.evalFuncValue(P.xs);
        v.push_back(P);


        {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(i+1));
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, 0, item);
        }

        for (unsigned int j = 0; j < grad.size(); j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(P.xs[j]));
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, j+1, item);
        }

        {
            //QTableWidgetItem *item = new QTableWidgetItem(QString::number(P.yt));
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(P.yt,'f',4));
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, grad.size()+1, item);
        }

        {
            QString s = "";
            QTableWidgetItem *item = new QTableWidgetItem(s);
            ui->tableWidget_2->setItem(i, grad.size()+2, item);
        }

        {
            QString s = "";
            QTableWidgetItem *item = new QTableWidgetItem(s);
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, grad.size()+3, item);
        }

        {
            QString s = "";
            QTableWidgetItem *item = new QTableWidgetItem(s);
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, grad.size()+4, item);
        }

        {
            QString s = "";
            QTableWidgetItem *item = new QTableWidgetItem(s);
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, grad.size()+5, item);
        }

        {
            QString s = "";
            QTableWidgetItem *item = new QTableWidgetItem(s);
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, grad.size()+6, item);
        }

        {
            QString s = "";
            QTableWidgetItem *item = new QTableWidgetItem(s);
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, grad.size()+7, item);
        }




        //-------------------------------------------------------------------------------
    }

    //double xxxxxxyy = ((ExperimentPoint)v[0]).yp;
    if(isFormulaExperiment)
    {
        for(int i = 0; i < ch; i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(((ExperimentPoint)v[i]).yp,'f',3));
            ui->tableWidget_2->setItem(i, grad.size()+2, item);
        }
    }
    ui->label_5->setText("");
    ui->label_6->setText("");
    this->initial = true;


    restartfindExtr();
    ui->tableWidget_2->clear();
    this->exec();

}

//-----------------------------------------------------------------------
void findExtrDialog::restartfindExtr()
{
    ui->pushButton_CCP->setDisabled(true);
    ui->pushButton_Experiment->setDisabled(true);
    initial = false;
    index1 = -1;
    index2 = -1;
    xP = 0;  //шаг, начиная с какого перевалило за 1
    xP_find = false;
    this->isExtremFound = false;

    //QTableWidgetItem* ptwi = 0;

    if(ui->radioButton->isChecked()) isMax = true;
        else isMax = false;
    h = ui->doubleSpinBox->value();
    ch = ui->spinBox_3->value();
    dev = ui->spinBox_2->value();

    isMax = table->getIsMax();
    if (isMax) {
        ui->radioButton->setChecked(true);
    } else {
        ui->radioButton_2->setChecked(true);
    }

    h = table->getStrideParameter();
    ui->doubleSpinBox->setValue(h);

    ch = table->getNumberStride();
    ui->spinBox_3->setValue(ch);

    dev = table->getInterestAllowedDeviation();
    ui->spinBox_2->setValue(dev);



    std::vector<bCoeff> bCoefs = this->table->b().coeffs();


    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount( this->table->b().count());
    ui->tableWidget->setRowCount(1);
    ui->tableWidget->verticalHeader()->hide();

    QStringList lst;
    for(int i = 0; i <  this->table->b().count(); i++)
    {
        lst << "β" + QString::number(i);
    }

    for(int i = 0; i < lst.size(); i++)
    {
        ui->tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(lst[i]));
    }

    for(int i = 0; i < this->table->b().count(); i++)
    {
        ui->tableWidget->setColumnWidth(i,65);
        //double xi = bCoefs[i].is_significant() ? bCoefs[i].natural_value() * (isMax ? 1.0 : -1.0) : 0.0;
        double xi =  bCoefs[i].natural_value() * (isMax ? 1.0 : -1.0);

        QTableWidgetItem *item = new QTableWidgetItem(QString::number(xi));
        item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
        ui->tableWidget->setItem(0, i, item);
        if (bCoefs[i].is_significant())
            ui->tableWidget->item(0, i)->setBackground(QColor(0, 255, 0, 127));
            //ui->tableWidget->item(0,i)->setBackground(Qt::green);
    }


    bCoefs.clear();
    bCoefs = this->table->b().coeffs();

    grad.clear();

    //перепишем b натуральные в градиент, при этом если незначим, то 0
    for(int i = 1; i < this->table->b().count(); i++)
    {
        //double xi = bCoefs[i].natural_value() * (isMax ? 1.0 : -1.0);
        double xi = bCoefs[i].is_significant() ? bCoefs[i].natural_value() * (isMax ? 1.0 : -1.0) : 0.0;
        grad.push_back(xi);
    }



    v_yp.clear();
    if(h_edit == false)
    {
        for(int i = 0; i < ui->tableWidget_2->rowCount(); i++)
        {
            v_yp.push_back(ui->tableWidget_2->item(i, grad.size()+2)->text());
        }
    }

    ui->tableWidget_2->clear();
    ui->tableWidget_2->setColumnCount(0);
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_2->setColumnCount( this->table->b().count()+7);
    ui->tableWidget_2->setRowCount(ch);
    ui->tableWidget_2->verticalHeader()->hide();

    lst.clear();
    lst << "№";
    for(int i = 1; i <= grad.size(); i++)
    {
        lst << "x" + QString::number(i);
    }
    lst << "Ygrad";
    lst << "Yreal";
    lst << "Ygrad-Yreal";
    lst << "Знак";

    lst << "Приращение\n по шагу";
    lst << "Ускорение или\n замедление\n движения к extr";

    lst << "% отклонения\n Ygrad от Yreal";

    int countlst = lst.size();

    for(int i = 0; i < lst.size(); i++)
    {
        ui->tableWidget_2->setColumnWidth(i,50);
        ui->tableWidget_2->setHorizontalHeaderItem(i, new QTableWidgetItem(lst[i]));
    }

    ui->tableWidget_2->setColumnWidth(0,20);


    ui->tableWidget_2->setColumnWidth(lst.size()-1,86);
    ui->tableWidget_2->setColumnWidth(lst.size()-2,90);
    ui->tableWidget_2->setColumnWidth(lst.size()-3,80);

    ui->tableWidget_2->setColumnWidth(lst.size()-4,42);
    ui->tableWidget_2->setColumnWidth(lst.size()-5,70);
    ui->tableWidget_2->setColumnWidth(lst.size()-6,60);
    ui->tableWidget_2->setColumnWidth(lst.size()-7,60);

    grad.clear();

    //перепишем b натуральные в градиент, при этом если незначим, то 0
    for(int i = 1; i < this->table->b().count(); i++)
    {
        double xi = bCoefs[i].is_significant() ? bCoefs[i].natural_value() * (isMax ? 1.0 : -1.0) : 0.0;
        grad.push_back(xi);
    }


    //Поиск max коэффициента в градиенте
    double max = fabs(grad[0]);

    for(unsigned int i = 0; i < grad.size(); i++)
        if (max < fabs(grad[i])) max = fabs(grad[i]);



    std::vector<double> vecH;    //вектор шагов

    for(unsigned int i = 0; i < grad.size(); i++)
    {
        vecH.push_back(h * grad[i] / max);
    }


    std::vector<double> vecHbuf;    //вектор шагов

    for(unsigned int i = 0; i < grad.size(); i++)
    {
        vecHbuf.push_back(0);
    }

    v.clear();

    //расчет xs и функции yt
    for (unsigned int i = 0; i < ch; i++)
    {
        if(xP_find == false)
        {
            xP = xP + h;
            if(xP > table->x().xDelta(0))
            {
                xP_find = true;
                xP = i;
            }
        }

        vecHbuf.clear();

        yt = bCoefs[0].natural_value();

        for (unsigned int j = 0; j < grad.size(); j++)
        {
            double buf = 0;
            buf = vecH[j] * (i + 1);
            vecHbuf.push_back(buf);
            yt = yt + grad[j] * buf;
        }

        P.xs.clear();
        P.xs = vecHbuf;

        for(size_t j = 0; j < P.xs.size(); j++)
        {
            if (bCoefs[j+1].is_significant())
            {
                if(bCoefs[j+1].index().size() == 1)
                {
                   //double xdelta = table->x().xDelta(j);
                   //double xcenter = table->x().xCenter(j);
                   int ccc =0;
                   P.xs[j] = table->x().xCenter(j) + table->x().xDelta(j) * P.xs[j];
                   int ccc2 =0;
                }
                else
                {
                    double buf = 0;
                    for(size_t k = 0; k < bCoefs[j+1].index().size(); k++)
                    {
                        int ccc =0;
                        buf += table->x().xCenter(bCoefs[j+1].index()[k]) + table->x().xDelta(bCoefs[j+1].index()[k]) * P.xs[j];
                        int ccc2 =0;
                    }
                    P.xs[j] = buf;
                }
            }
            else P.xs[j] = 0;
        }

        std::vector<double> yy = src->getYdata2(P.xs);
        int xsize = yy.size();
        if (yy.size()!=0)
            P.yp = yy[0];
        else
            P.yp = table->evalFuncValue(P.xs);

        P.yt = this->table->evalFuncValue(P.xs);
        v.push_back(P);


        {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(i+1));
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, 0, item);
        }

        for (unsigned int j = 0; j < grad.size(); j++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(P.xs[j],'f',3));
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, j+1, item);
        }

        {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(P.yt,'f',3));
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, grad.size()+1, item);
        }

        {
            QString s = "";
            QTableWidgetItem *item = new QTableWidgetItem(s);
            ui->tableWidget_2->setItem(i, grad.size()+2, item);
        }

        {
            QString s = "";
            QTableWidgetItem *item = new QTableWidgetItem(s);
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, grad.size()+3, item);
        }

        {
            QString s = "";
            QTableWidgetItem *item = new QTableWidgetItem(s);
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, grad.size()+4, item);
        }

        {
            QString s = "";
            QTableWidgetItem *item = new QTableWidgetItem(s);
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, grad.size()+5, item);
        }

        {
            QString s = "";
            QTableWidgetItem *item = new QTableWidgetItem(s);
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, grad.size()+6, item);
        }

        {
            QString s = "";
            QTableWidgetItem *item = new QTableWidgetItem(s);
            item->setFlags(item->flags()& ~Qt::ItemIsEditable);//read only
            ui->tableWidget_2->setItem(i, grad.size()+7, item);
        }
    }

    this->initial = true;
    ui->label_4->setText("");

    ui->label_5->setText("");

    if(this->xP_find)
        ui->label_5->setText("Шаг на котором вышли за пределы эксперимента " + QString::number(this->xP+1));
    else
        ui->label_5->setText("Не вышли за пределы эксперимента");

/*
    std::vector<bool> znakPol;
    P = v[0];
    for (int j = 0; j < P.xs.size(); j++)
    {
        if(P.xs[j]>=0)
        {
            znakPol.push_back(true);
        }
        else
        {
             znakPol.push_back(false);
        }
    }
    QString s = "";
    for (int i = 1; i < v.size(); i++)
    {
       P = v[i];
       for (int j = 0; j < P.xs.size(); j++)
       {
           if(P.xs[j]>0)
           {
               if (znakPol[j]!=true)
                   s = s+"x"+QString::number(j)+"  ";
           }
           else
           {
               if (znakPol[j]!=false)
                   s = s+"x"+QString::number(j)+"  ";
           }
       }
    }
*/
    ui->label_6->setText("");
   // ui->label_6->setText(s);

    for(int i = 0; i < ch; i++)
    {
        ui->tableWidget_2->item(i, 0)->setBackground(Qt::white);
    }


    if(h_edit == false)
    {
        if (v_yp.size()<=ch)
        {
            for(int i = 0; i < v_yp.size(); i++)
            {
                ui->tableWidget_2->item(i,grad.size()+2)->setText(v_yp[i]);
            }
        }
        else
        {
            for(int i = 0; i < ch; i++)
            {
                ui->tableWidget_2->item(i,grad.size()+2)->setText(v_yp[i]);
            }
        }
    }



    if(isFormulaExperiment)
    {
        for(int i = 0; i < ch; i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(((ExperimentPoint)v[i]).yp,'f',3));
            ui->tableWidget_2->setItem(i, grad.size()+2, item);
        }
    }

    h_edit = false;
}

void findExtrDialog::updateTable()
{
    std::vector<double> grad;
    std::vector<bCoeff> bCoefs = table->b().coeffs();


    //перепишем b натуральные в градиент, при этом если незначим, то 0
    for(int i = 1; i < table->b().count(); i++)
    {
        double xi = bCoefs[i].is_significant() ? bCoefs[i].natural_value() * (isMax ? 1.0 : -1.0) : 0.0;
        grad.push_back(xi);
    }

    for(int i = 0; i < ch; i++)
    {
        bool isOk;
        bool isOk2;

        double Ypp = ui->tableWidget_2->item(i, grad.size()+2)->text().toDouble(&isOk);
        double Ytt = ui->tableWidget_2->item(i, grad.size()+1)->text().toDouble(&isOk2);

        int l = ui->tableWidget_2->item(i,grad.size()+2)->text().length();//надо заменить проверку, а то эта кривая


        if(l>0)
        {
            //ui->label->setText((QString::number(val2-val1)));
            //ui->tableWidget_2->item(i, grad.size()+3)->setText((QString::number(val2-val1)));

            this->v[i].yp = Ypp;

            //QFont font1;
            //font1.setPointSize(10);
            //font1.setBold(true);
            //ui->tableWidget_2->item(i, grad.size()+4)->setFont(font1);
            ui->tableWidget_2->item(i, grad.size()+4)->setTextAlignment(Qt::AlignCenter);

            if (isMax == true)
            {

                ui->tableWidget_2->item(i, grad.size()+3)->setText((QString::number((Ypp - Ytt),'f',3)));
                if ((Ypp - Ytt) > 0)
                {
                    //ui->tableWidget_2->item(i, grad.size()+4)->setText("+");
                    ui->tableWidget_2->item(i, grad.size()+4)->setText("к extr");
                    //ui->tableWidget_2->item(i, grad.size()+4)->setBackground(Qt::green);
                    ui->tableWidget_2->item(i, grad.size()+4)->setBackground(QColor(0, 255, 0, 127));
                }
                else
                {
                    //ui->tableWidget_2->item(i, grad.size()+4)->setText("-");
                    ui->tableWidget_2->item(i, grad.size()+4)->setText("от extr");
                    //ui->tableWidget_2->item(i, grad.size()+4)->setBackground(Qt::red);
                    ui->tableWidget_2->item(i, grad.size()+4)->setBackground(QColor(255, 0, 0, 127));
                }
            }
            else
            {
                ui->tableWidget_2->item(i, grad.size()+3)->setText((QString::number((Ytt - Ypp),'f',3)));
                if ((Ytt - Ypp) > 0)
                {
                    //ui->tableWidget_2->item(i, grad.size()+4)->setText("-");
                    ui->tableWidget_2->item(i, grad.size()+4)->setText("к extr");
                    //ui->tableWidget_2->item(i, grad.size()+4)->setBackground(Qt::green);
                    ui->tableWidget_2->item(i, grad.size()+4)->setBackground(QColor(0, 255, 0, 127));
                }

                else
                {
                    //ui->tableWidget_2->item(i, grad.size()+4)->setText("+");
                    ui->tableWidget_2->item(i, grad.size()+4)->setText("от extr");
                    //ui->tableWidget_2->item(i, grad.size()+4)->setBackground(Qt::red);
                    ui->tableWidget_2->item(i, grad.size()+4)->setBackground(QColor(255, 0, 0, 127));
                }
            }


            //оклонение приращения
            if (i!=0)
            {
                double a = ui->tableWidget_2->item(i, grad.size()+2)->text().toDouble(&isOk);
                double b = ui->tableWidget_2->item(i-1, grad.size()+2)->text().toDouble(&isOk2);


                ui->tableWidget_2->item(i, grad.size()+5)->setText((QString::number(fabs(a - b),'f',3)));


                QFont font1;
                font1.setPointSize(14);
                font1.setBold(true);
                ui->tableWidget_2->item(i, grad.size()+6)->setFont(font1);
                ui->tableWidget_2->item(i, grad.size()+6)->setTextAlignment(Qt::AlignCenter);


                if ((a - b) >= 0)
                {
                    ui->tableWidget_2->item(i, grad.size()+6)->setText("+");
                    //ui->tableWidget_2->item(i, grad.size()+6)->setBackground(Qt::green);
                    ui->tableWidget_2->item(i, grad.size()+6)->setBackground(QColor(0, 255, 0, 127));
                }
                else
                {
                    ui->tableWidget_2->item(i, grad.size()+6)->setText("-");
                    //ui->tableWidget_2->item(i, grad.size()+6)->setBackground(Qt::red);
                    ui->tableWidget_2->item(i, grad.size()+6)->setBackground(QColor(255, 0, 0, 127));
                }
            }

            double Ypp = ui->tableWidget_2->item(i, grad.size()+2)->text().toDouble();
            double Ytt = ui->tableWidget_2->item(i, grad.size()+1)->text().toDouble();
            double dev2 = fabs((Ypp / Ytt) - 1) * 100;

            ui->tableWidget_2->item(i, grad.size()+7)->setText((QString::number(dev2,'f',3)));

            if (dev2 < this->dev)
            {
                //ui->tableWidget_2->item(i, grad.size()+7)->setBackground(Qt::green);
                ui->tableWidget_2->item(i, grad.size()+7)->setBackground(QColor(0, 255, 0, 127));
            }
            else
            {
                //ui->tableWidget_2->item(i, grad.size()+7)->setBackground(Qt::red);
                 ui->tableWidget_2->item(i, grad.size()+7)->setBackground(QColor(255, 0, 0, 127));
            }
        }
    }
}

void findExtrDialog::findExtr()
{
/*
    //ннет точки перегиба
    (v[0]).yt=249.478;
    (v[1]).yt=362.982;
    (v[2]).yt=476.485;
    (v[3]).yt=589.988;
    (v[4]).yt=703.492;
    (v[5]).yt=816.995;
    (v[6]).yt=930.498;

    (v[0]).yp=250;
    (v[1]).yp=363;
    (v[2]).yp=477;
    (v[3]).yp=590;
    (v[4]).yp=784;
    (v[5]).yp=817;
    (v[6]).yp=931;
*/
    /*
    //есть точка перегиба
        (v[0]).yt=249.478;
        (v[1]).yt=362.982;
        (v[2]).yt=476.485;
        (v[3]).yt=589.988;
        (v[4]).yt=580.492;
        (v[5]).yt=570.995;
        (v[6]).yt=560.498;

        (v[0]).yp=250;
        (v[1]).yp=363;
        (v[2]).yp=477;
        (v[3]).yp=590;
        (v[4]).yp=581;
        (v[5]).yp=571;
        (v[6]).yp=561;
*/
/*
    //нет точки перегиба но есть оклонение большое
    (v[0]).yt=249.478;
    (v[1]).yt=362.982;
    (v[2]).yt=476.485;
    (v[3]).yt=589.988;
    (v[4]).yt=703.492;
    (v[5]).yt=816.995;
    (v[6]).yt=930.498;

    (v[0]).yp=250;
    (v[1]).yp=363;
    (v[2]).yp=477;
    (v[3]).yp=590;
    (v[4]).yp=784;
    (v[5]).yp=10817;
    (v[6]).yp=10931;
    */
/*
    //есть точка перегиба и есть отклонение
        (v[0]).yt=249.478;
        (v[1]).yt=362.982;
        (v[2]).yt=476.485;
        (v[3]).yt=589.988;
        (v[4]).yt=58.492;
        (v[5]).yt=57.995;
        (v[6]).yt=56.498;

        (v[0]).yp=250;
        (v[1]).yp=363;
        (v[2]).yp=477;
        (v[3]).yp=590;
        (v[4]).yp=580;
        (v[5]).yp=570;
        (v[6]).yp=560;
*/
    //определим есть ли точка перегиба
        int tp = -1;

        for(size_t i = xP; i < v.size(); i++)
        {
            if (isMax == true)
            {
                if((v[i - 1]).yp > (v[i]).yp)
                {
                    //есть точка перегиба, max
                    tp = i - 1;
                    break;
                }
            }
            else
            {
                if((v[i - 1]).yp < (v[i]).yp)
                {
                    //есть точка перегиба, min
                    tp = i - 1;
                    break;
                }
            }
        }

        if(tp != -1)
        {
            //если есть точка перегиба, то получаем практические значения y на каждом шаге

            bool prev = false;
            //после расчета практических знчений определим есть превышение допустимого отклонения yt от yp
            for(size_t i = xP; i < v.size(); i++)
            {
                if(dev < (fabs(((v[i]).yt / (v[i]).yp) - 1) * 100)) //((yt/yp)-1)*100% - перевод в проценты
                {
                    //есть превышение допустимого отклонения yt от yp

                    //проводим эксперимент для получения уточненного направления градиента в этой точке
                    v.erase (v.begin() + i + 1, v.end()); //если есть превышение, то удалим все после мах

                    isExtremFound = false;

                    int xxx = 111;
                    xxx = 111;

                    //return v;
                    QString s = "Превышение допустимого отклонения "+QString::number(i+1);;
                    ui->label_4->setText(s);
                    ui->tableWidget_2->item(i, 0)->setBackground(Qt::green);
                    prev = true;
                    break;

                }
            }

            if(prev==false)
            {
                //если нет превышения допустимого отклонения, то return
                v.erase (v.begin() + tp + 1, v.end());
                isExtremFound = true;

                int xxx = 222;
                xxx = 222;

                //return v;// успех
                QString s = ""+QString::number(tp+xP-2);
                ui->label_4->setText("Точка перегиба на шаге "+s);
                ui->tableWidget_2->item(tp+xP-2, 0)->setBackground(Qt::green);
            }
        }
        else
        {
            //если нет точки перегиба, то проводим эксперимент в последней точке
            QString s = "";

            int xI; // шаг на котором max отклонение yt и yp
            double max2 = (fabs(((v[xP]).yt / (v[xP]).yp) - 1) * 100);
            xI = xP;

            bool prev = false;

            //после расчета практических знчений определим есть превышение допустимого отклонения yt от yp
            for(size_t i = xP; i < v.size(); i++)
            {
                //double buf_dev = (fabs(((v[i]).yt / (v[i]).yp)-1)*100);
                if(dev < (fabs(((v[i]).yt / (v[i]).yp) - 1) * 100))
                {
                    //есть превышение допустимого отклонения yt от yp
                    if(max2 < (fabs(((v[i]).yt / (v[i]).yp) - 1) * 100))
                    {
                        max2 = (fabs(((v[i]).yt / (v[i]).yp) - 1) * 100);
                        xI = i;
                        prev = true;
                        s = ", но есть прев. допуст. оклон. в точке " +QString::number(xI+xP-1);
                    }
                }
            }

            if(prev == true) v.erase (v.begin() + xI + 1, v.end()); //если есть превышение, то удалим все после мах

            isExtremFound = false;

            int xxx = 333;
            xxx = 333;


            ui->label_4->setText(s);
            ui->tableWidget_2->item(v.size()-1, 0)->setBackground(Qt::green);

            //return v;
        }
}

bool findExtrDialog::calcYpp()
{
    int count = 0;
    for(int i = 0; i < ch; i++)
    {
        int l = ui->tableWidget_2->item(i,grad.size()+2)->text().length();
        if(l>0)
        {
            count++;
        }
    }

    if (count==ch)
        return true;
    else return false;
}

void findExtrDialog::on_pushButton_clicked()
{
    h_edit = true;
    restartfindExtr();
}

void findExtrDialog::on_tableWidget_2_cellChanged()
{
    if(initial == true)
    {
        ui->label_4->setText("");

        updateTable();

        if ((calcYpp() == true)&&(!isExtremFound))
        {
            for(int i = 0; i < ch; i++)
            {
                (v[i]).yp = ui->tableWidget_2->item(i, grad.size()+2)->text().toDouble();
            }
            //findExtr();
        }
    }
}

void findExtrDialog::on_pushButton_CCP_clicked()
{
    if (index1 != -1)
    {
        this->ccp = true;
        this->exper = false;
        v.erase (v.begin() + this->index1+1, v.end());
        this->close();
    }
}

void findExtrDialog::on_pushButton_Experiment_clicked()
{
    if (index1 != -1)
    {
        this->exper = true;
        this->ccp = false;
        if(index2==-1)
            v.erase (v.begin() + this->index1+1, v.end());
        else
        {
            ExperimentPoint P1,P2,P3;
            P1 = (ExperimentPoint)v[index1];
            P2 = (ExperimentPoint)v[index2];
            for(int i = 0; i < P1.xs.size(); i++)
            {
                P3.xs.push_back((P1.xs[i]+P2.xs[i])/2);
            }
            v.clear();
            v.push_back(P3);
        }
        this->close();
    }
}

void findExtrDialog::on_doubleSpinBox_editingFinished()
{
    h_edit = true;
}

void findExtrDialog::on_radioButton_clicked()
{
    h_edit = true;
}

void findExtrDialog::on_radioButton_2_clicked()
{
    h_edit = true;
}


void findExtrDialog::on_tableWidget_2_doubleClicked(const QModelIndex &index)
{
    ui->pushButton_CCP->setDisabled(false);
    ui->pushButton_Experiment->setDisabled(false);
    int prev_index = this->index1;

    if(this->index1==-1)
        this->index1 = index.row();
    else
    {
        if(fabs(index.row()-this->index1)<2)
        {
            this->index2 = this->index1;
            this->index1 = index.row();
        }
        else
        {
            if(fabs(index.row()-this->index2)<2)
            {
                this->index1 = index.row();
            }
            else
            {
                this->index1 = index.row();
                this->index2 = -1;
            }
        }
    }

    for (int j = 0; j < ch; j++)
    {
        for(int i = 0; i <= grad.size()+1; i++)
        {
            ui->tableWidget_2->item(j ,i)->setBackground(Qt::white);
        }
    }

    for(int i = 0; i <= grad.size()+1; i++)
    {
        ui->tableWidget_2->item(this->index1 ,i)->setBackground(QColor(30,144,255));
    }

    if(index2!=-1)
    for(int i = 0; i <= grad.size()+1; i++)
    {
        ui->tableWidget_2->item(this->index2 ,i)->setBackground(QColor(30,144,255));
    }

    if(index2==-1)
        ui->label_4->setText("Выбран шаг " + QString::number(this->index1+1));
    else
        if(this->index1!=this->index2)
            ui->label_4->setText("Выбран шаг " + QString::number(this->index1+1) +" и " + QString::number(this->index2+1));
        else
            ui->label_4->setText("Выбран шаг " + QString::number(this->index1+1));
}
void findExtrDialog::on_pushButton_2_clicked()
{
    table->setIsMax(ui->radioButton->isChecked());
    table->setStrideParameter(ui->doubleSpinBox->value());
    table->setNumberStride(ui->spinBox_3->value());
    table->setInterestAllowedDeviation(ui->spinBox_2->value());

    QString fileName = QFileDialog::getSaveFileName(this, ("Сохранить"), "new_experiment.csv", ("CSV(*.csv);;All Files(*)"));
//    this->table->save(fileName.toAscii().data());
    this->saveToCSV(fileName);

}
void findExtrDialog::saveToCSV(const QString &fileName)
{
    this->table->save(fileName.toAscii().data());
}
