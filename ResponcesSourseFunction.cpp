#include "ResponcesSourseFunction.h"

ResponcesSourseFunction::ResponcesSourseFunction(): FactNum(0), PFEnum(0), ActualFactNum(0), MinFactNum(0)
{
    //RunScript = new QScriptEngine();
}
ResponcesSourseFunction::~ResponcesSourseFunction()
{
    //delete RunScript;
}

double ResponcesSourseFunction::centerFor(int xIndex)
{
    return FactValues.at(xIndex);
}

// возвращает интервал вырьирования для указаного № фактора
void ResponcesSourseFunction::intervalFor(int xIndex, double *xMin, double *xMax)
{
    *xMin = FactDivergences.at(xIndex);
    *xMax = FactDivergences.at(xIndex);


}

//получить все || значения опытов для указаных нормальных значений факторов xVal для каждого row -> getData -> получаю Y с которыми работаю
std::vector<double> ResponcesSourseFunction::getYdata(std::vector<int> normCords)
{
    if (EvaluateFunction.at(EVALUATE_STRING)!="")
    {
        QString varstring = "";
        QScriptEngine RunScript;

        for (int i = 0; i < FactNum; i++)
        {
            double val = FactValues.at(i) + (FactDivergences.at(i) / 2) * normCords.at(i);
            // varstring += Descriptions.at(i) + " = " + QString::number(val) + " ; ";
            varstring += "x" + QString::number(i+1) + " = " + QString::number(val) + " ; ";
        }

        QString tmp = EvaluateFunction.at(EVALUATE_STRING);
        QString runstr="with (Math) {" + varstring + tmp.replace("\"","") + "}";
        QScriptValue ScriptRes = RunScript.evaluate(runstr);
        QString sansw = ScriptRes.toString(); //"Infinity"
        if (sansw=="Infinity"|| sansw=="NaN")
        {
            QString warning_text="";
            warning_text+=QString::fromUtf8("<p>Невозможно вычислить значение функции:</p>");
            warning_text+=EvaluateFunction.at(0);
            warning_text+=QString::fromUtf8("<p>при укаазнных значениях переменных</p>");
            warning_text+=QString::fromUtf8("<table border=1><tr><td>Фактор</td><td>Значение</td></tr>");
            QStringList errXparams= varstring.split(';');
            for (int i=0;i<errXparams.length();i++)
            {
                if (errXparams.at(i).length()>3)
                {
                QString XNum=errXparams.at(i).split('=').at(0);
                QString XVal=errXparams.at(i).split('=').at(1);
                warning_text+="<tr><td>"+XNum+"</td><td>"+XVal+"</td></tr>";
                }
            }
            warning_text+=QString::fromUtf8("<tr><td>Значение функции</td><td>Бесконечность</td></tr></table>");

            //QMessageBox::about(this,QString::fromUtf8("Невозможно вычислить значение функии"),warning_text);
            QMessageBox msg;
            msg.setText(warning_text);
            msg.setWindowTitle(QString::fromUtf8("Невозможно вычислить значение функии"));
            msg.setSizeGripEnabled(true);
            msg.exec();
        }
        double dansw = ScriptRes.toString().toDouble();
        std::vector<double> res;
        res.push_back(dansw);
        return res;
    }
    else
    {
         return std::vector<double>(parall);
    }
}

//получить все || значения опытов для указаных нормальных значений факторов xVal для каждого row -> getData -> получаю Y с которыми работаю
std::vector<double> ResponcesSourseFunction::getYdata2(std::vector<double> normCords)
{
    if (EvaluateFunction.at(EVALUATE_STRING)!="")
    {
        QString varstring = "";
        QScriptEngine RunScript;

        for (int i = 0; i < FactNum; i++)
        {
            //double val = FactValues.at(i) + (FactDivergences.at(i) / 2) * normCords.at(i);
            varstring += "x" + QString::number(i+1) + " = " + QString::number(normCords.at(i)) + " ; ";
        }

        QString runstr="with (Math) {" + varstring + EvaluateFunction.at(EVALUATE_STRING) + "}";
        QScriptValue ScriptRes = RunScript.evaluate(runstr);
        QString sansw = ScriptRes.toString(); //"Infinity"
        if (sansw=="Infinity")
        {
            QString warning_text="";
            warning_text+=QString::fromUtf8("<p>Невозможно вычислить значение функции:</p>");
            warning_text+=EvaluateFunction.at(0);
            warning_text+=QString::fromUtf8("<p>при укаазнных значениях переменных</p>");
            warning_text+=QString::fromUtf8("<table border=1><tr><td>Фактор</td><td>Значение</td></tr>");
            QStringList errXparams= varstring.split(';');
            for (int i=0;i<errXparams.length();i++)
            {
                if (errXparams.at(i).length()>3)
                {
                QString XNum=errXparams.at(i).split('=').at(0);
                QString XVal=errXparams.at(i).split('=').at(1);
                warning_text+="<tr><td>"+XNum+"</td><td>"+XVal+"</td></tr>";
                }
            }
            warning_text+=QString::fromUtf8("<tr><td>Значение функции</td><td>Бесконечность</td></tr></table>");

            QMessageBox msg;
            msg.setText(warning_text);
            msg.setWindowTitle(QString::fromUtf8("Невозможно вычислить значение функии"));
            msg.setSizeGripEnabled(true);
            msg.exec();
        }
        double dansw = ScriptRes.toString().toDouble();
        std::vector<double> res;
        res.push_back(dansw);
        return res;
    }
    else
    {
         return std::vector<double>(parall);
    }
}

int ResponcesSourseFunction::inputsCount() const
{
    return FactNum;
}

//общее число факторов
int ResponcesSourseFunction::actualInputsCount() const
{
    return ActualFactNum;
}
//число "базовых", основных факторов. Остальные - комбинация базовых.

QStringList ResponcesSourseFunction::getDescriptions() const
{
    return Descriptions;
}

QString ResponcesSourseFunction::CheckFormulaForTrinometricFunctions(QString text)
{
    QString FunctionName="";
    QString FunctionParams="";
    QString FunctionReg = "^(sin\\(|asin\\(|cos\\(|acos\\(|tan\\(|atan\\(|ctan\\()";
    QString res="";
    while (text!="")
    {
        FunctionName=CheckExpressionPattern(FunctionReg,text);
        if (FunctionName!="")
        {
            QString ParamStr=text.mid(FunctionName.length()-1,1+text.length()-FunctionName.length());
            FunctionParams=GetTrigonometricFuncParams(ParamStr);
            QString Apdatedparams= CheckFormulaForTrinometricFunctions(FunctionParams.mid(1,FunctionParams.length()-2));
            res+=FunctionName+"(PI/180)*("+Apdatedparams+"))";
            int poscount=(FunctionName.length()-1)+FunctionParams.length();
            if (poscount>text.length()) poscount=text.length();
            text=text.replace(0,poscount,"");
        }
        else
        {
            res+=text.at(0);
            text=text.replace(0,1,"");
        }
    }
    return res;
}

QString ResponcesSourseFunction::GetTrigonometricFuncParams(QString text)
{
    int bracketsCount=-1;
    int position=0;
    QString res="";

    while ((bracketsCount!=0) &&(position<text.length()))
    {
            QString Symbol=text.at(position);
            if (Symbol=="(")
            {
                if (bracketsCount==-1) bracketsCount=0;
                bracketsCount++;
            }
            else if (Symbol==")")
            {
                if (bracketsCount==-1) bracketsCount=0;
                bracketsCount--;
            }
            position++;
      }
    res= text.mid(0,position);
    return res;
}

QString ResponcesSourseFunction::CheckExpressionPattern( QString pattern, QString text)
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

bool ResponcesSourseFunction::RecognizeDomChildInfo(QDomElement ch1,int Parrent)
{
    // разбор xml
    if (ch1.text()!="")
    {
        if (ch1.tagName()=="text" && ch1.parentNode().nodeName()=="formula")
        {
            if (this->EvaluateFunction.count()>0)
                this->EvaluateFunction[0]=ch1.text();
            else
                this->EvaluateFunction<<ch1.text();
        }
        else if (ch1.tagName()=="corner" && ch1.parentNode().nodeName()=="formula")
        {
            if (EvaluateFunction.count()>0)
            {
                if (this->EvaluateFunction.at(0)!="")
                {
                    QString Eval=this->EvaluateFunction.at(0);
                    if (ch1.text()=="degrees")
                    {
                        Eval="/*degrees*/"+this->CheckFormulaForTrinometricFunctions(Eval);
                    }
                    SetEvaluateFunction(this->EvaluateFunction.at(0),Eval);
                }
            }
        }
        else if (ch1.tagName()=="factors" && ch1.parentNode().nodeName()=="settings")
        {
            this->FactNum=ch1.childNodes().count();
            this->ActualFactNum=this->FactNum;
            this->MinFactNum=1;
            this->MaxDrobRepl=this->FactNum;
            this->DrobRepl=1;
            for (int i=0;i<this->FactNum;i++)
            {
                this->FactValues.push_back(0);
                this->FactDivergences.push_back(0);
            }
        }
        else if (ch1.tagName()=="center" && ch1.parentNode().nodeName().toLower().contains("x"))
        {
            int number = ch1.parentNode().nodeName().toLower().remove("x").toInt();
            FactValues[number-1]=ch1.text().toDouble();
        }
        else if (ch1.tagName()=="divergences" && ch1.parentNode().nodeName().toLower().contains("x"))
        {
            int number = ch1.parentNode().nodeName().toLower().remove("x").toInt();
            FactDivergences[number-1]=ch1.text().toDouble();
        }

    for (int i=0;i<ch1.childNodes().count();i++)
    {
        QDomElement ch2 = ch1.childNodes().at(i).toElement();
        RecognizeDomChildInfo(ch2,i);
    }
    return true;
    }
}


bool ResponcesSourseFunction::LoadFormula(QString FileName)
{
    QDomDocument dom;

    QXmlStreamReader xml;
    QFile file(FileName);
    if (file.open(QIODevice::ReadWrite))
    {
        dom.setContent(&file);     
        for (int i=0;i<dom.childNodes().count();i++)
        {
             QDomElement ch1=dom.childNodes().at(i).toElement();
             RecognizeDomChildInfo(ch1,0);
        }
        return true;
    }
    return true;
}


bool ResponcesSourseFunction::SaveFormula(QString FileName)
{
    if (this->EvaluateFunction.length() <=0 && this->FactNum <=0)
        return false;

    QXmlStreamWriter xml;
    QFile file(FileName);
    if (file.open(QIODevice::ReadWrite))
    {

        QTextStream ts(&file);

    xml.setDevice(ts.device());
    QString corner_metric="radians";

    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("settings");
    if (this->EvaluateFunction.length()>0)
    {
        xml.writeStartElement("formula");
            xml.writeTextElement("text",this->EvaluateFunction.at(0));
            if (EvaluateFunction.at(1).contains("/*degrees*/"))
            {
                corner_metric="degrees";
            }
            xml.writeTextElement("corner",corner_metric);
        xml.writeEndElement(); //formula
    }

        if (this->FactNum>0)
        {
            xml.writeStartElement("factors");

        for (int i=0;i<this->FactNum;i++)
        {
            xml.writeStartElement("x"+QString::number(i+1));
            xml.writeTextElement("center",QString::number(this->FactValues.at(i)));
            xml.writeTextElement("divergences",QString::number(this->FactDivergences.at(i)));
            xml.writeEndElement();
        }
        xml.writeEndElement(); //factors
        }
    xml.writeEndElement(); //settings
    xml.writeEndDocument();
    return true;
    }
    else
    {
        return false;
    }
}
