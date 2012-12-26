#include "UiController.h"
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
#include "ExperimentDialog.h"
#include "bCoeff.h"
#include "findExtrDialog.h"
#include "savedirectorypathdialog.h"

#include <assert.h>


#include "NewExperimentSettingsDialog.h"
UiController::UiController(IView *view): _dataSrc(NULL), _formatterView(new NumberFormatDialog), _formatter(NULL), _view(view)
{
    _modelHasData = false;
    _experimentTable = NULL;
    _interactionLevel = 1;
    _isFormulaModel = false;
    _paral = 0;
    _ModelType = MATHMODEL;
}

bool UiController::newModel(short ModelType,bool createEmpty, bool isFormulaModel)
{
    _experimentTable = NULL;
    _modelHasData = false;
    _interactionLevel = 1;
    _paral = 0;
    _isFormulaModel =true;// isFormulaModel;
    _ModelType = ModelType;

    delete _dataSrc;
    _dataSrc = NULL;

    if(createEmpty)
    {
        initEmptyModel();
        return true;
    }
    else
    {
        bool res=   changeModelParams();
        return res;
    }
}

void UiController::newModellGrad(bool _isFormulaModel2)
{
    _experimentTable = NULL;
    _modelHasData = false;
    _interactionLevel = 1;

    _isFormulaModel =_isFormulaModel2;// isFormulaModel;

    if(!_isFormulaModel2)
        changeModelParamsGrad();
    else
        changeModelParams();
}

UiController::~UiController()
{
    delete _formatter;
    delete _dataSrc;
    delete _formatterView;
    delete _experimentTable;
}

bool UiController::NewExperimenLoadfromCSV(const QString &fileName)
{
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly))
        return false;
    QTextStream srcStream(&file);

    /* очень нужная штука для расшифровки кодировки*/
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    if (codec!= NULL)
    {
        srcStream.setCodec(codec);
    }
    else
    {
        assert(1);
    }
    /* очень нужная штука для расшифровки кодировки*/

    QStringList evaluateFunction;
    int replicaDelimiter;
    int factorCount;
    int generalFactorCount;
    int replica_row_count;
    QString tmp_str = srcStream.readLine();
    QString function = tmp_str.split(QRegExp(";"))[1];
    QString measure = "";
    if ("" != function) {
        measure = srcStream.readLine().split(QRegExp(";"))[1];
        evaluateFunction << function << measure;
    } else {
        evaluateFunction << "" << "";
    }

    QString tmp;
    QStringList descriptions;
    QStringList tmpList;
    std::vector<double> *center;
    std::vector<double> *delta;
    double num;
    tmp = srcStream.readLine();
    if ("" == tmp.split(QRegExp(";"))[1]) {
        tmp = srcStream.readLine();
    }
    if ("" != tmp) {
        tmp = tmp.split(QRegExp(";"))[1];
        factorCount = tmp.toInt();
        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        replica_row_count = tmp.toInt();
        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        replicaDelimiter = tmp.toInt();
        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        generalFactorCount = tmp.toInt();

        srcStream.readLine();

        center = new std::vector<double>(factorCount);
        delta  = new std::vector<double>(factorCount);
        for(size_t i = 0; i < factorCount; i++)
        {
            tmpList = srcStream.readLine().split(QRegExp(";"));
            tmp = ExperimentTable::doubleWithDot(tmpList[1]);
            num = tmp.toDouble();
            center->at(i) = num;
        }
        srcStream.readLine();
        for(size_t i = 0; i < factorCount; i++)
        {
            tmpList = srcStream.readLine().split(QRegExp(";"));
            tmp = ExperimentTable::doubleWithDot(tmpList[1]);
            num = tmp.toDouble();
            delta->at(i) = num*2;
        }

        quint64 descrCount;
        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        descrCount = tmp.toULongLong();
        srcStream.readLine();
        if(descrCount > 0)
        {
            for(size_t i = 0; i < descrCount; i++)
            {
                tmpList = srcStream.readLine().split(QRegExp(";"));
                QString d = ExperimentTable::doubleWithDot(tmpList[1]);;
                descriptions << d.replace("\"","");
            }
        }
        QString intLevString = srcStream.readLine().split(QRegExp(";"))[1];
        _interactionLevel = intLevString.toInt();
    }
    // Model Type
    bool isMax = true;
    double strideParameter = 0.50;
    int numberStride = 7;
    double interestAllowedDeviation = 30;
    tmp = srcStream.readLine();
    tmp = tmp.split(QRegExp(";"))[1];
    if("MATHMODEL" == tmp)
    {
        _ModelType = MATHMODEL;
    }
    else
    {
        srcStream.readLine();
        tmp = srcStream.readLine();
        tmp = tmp.split(QRegExp(";"))[1];
        bool isMax = tmp.toInt();
        tmp = srcStream.readLine();
        tmp = ExperimentTable::doubleWithDot(tmp.split(QRegExp(";"))[1]);
        double strideParameter = tmp.toDouble();
        tmp = srcStream.readLine();
        tmp = tmp.split(QRegExp(";"))[1];
        int numberStride = tmp.toInt();
        tmp = srcStream.readLine();
        tmp = ExperimentTable::doubleWithDot(tmp.split(QRegExp(";"))[1]);
        double interestAllowedDeviation = tmp.toDouble();
        _ModelType = EXTRSEARCH;
    }


    quint64 size;
    vector<vector<YInfo> > m_values;
    tmp = srcStream.readLine();
    if ("" != tmp) {
        tmp = tmp.split(QRegExp(";"))[1];
        _paral = tmp.toULongLong();

        srcStream.readLine();
        ResponcesSourseFunction *src = new ResponcesSourseFunction();
        src->ActualFactNum = generalFactorCount;
        src->FactNum = factorCount;
        src->MinFactNum = -1;
        src->parall = _paral;
        src->PFEnum = pow(2., src->FactNum);
        src->Descriptions = descriptions;
        src->FactDivergences = *delta;
        src->FactValues = *center;
        if ("" == function)
        {
            src->SetEvaluateFunction("","");
        }
        else
        {
            QString tmpstr=function;
            if ("degrees" == measure)
            {
                tmpstr=src->CheckFormulaForTrinometricFunctions(tmpstr);
            }
            src->SetEvaluateFunction(function,tmpstr);
        }
        src->DrobRepl=2./3;
        _dataSrc = src;

        _experimentTable = ExperimentTable::createExperimentTable(ReplicaGradient, factorCount, replicaDelimiter, _interactionLevel, evaluateFunction.at(0), evaluateFunction.at(1), _ModelType);

        _experimentTable->setIsMax(isMax);
        _experimentTable->setStrideParameter(strideParameter);
        _experimentTable->setNumberStride(numberStride);
        _experimentTable->setInterestAllowedDeviation(interestAllowedDeviation);

        _experimentTable->x().setFactorsDescriptions(descriptions);

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
        if ("" == evaluateFunction.at(0)) {
        m_values.resize(replica_row_count);
            for(size_t i = 0; i < replica_row_count; i++)
            {
                tmpList = srcStream.readLine().split(QRegExp(";"));
                if (_paral + 1 == tmpList.length()) {
                    for(size_t j = 0; j < _paral; j++)
                    {
                        double value;
                        bool IsTrusted;
                        tmp = ExperimentTable::doubleWithDot(tmpList[1 + j]);
                        value = tmp.toDouble();
                        YInfo yi;
                        yi.Value = value;
                        yi.IsTrusted = true;
                        m_values[i].push_back(yi);
                    }
                    _experimentTable->y().set_at(i, m_values[i]); //add it to table
                }
                else
                {
                    for(size_t j = 0; j < _paral; j++)
                    {
                        double value;
                        bool IsTrusted;
                        if (tmpList.length() > j+1) {
                            tmp = ExperimentTable::doubleWithDot(tmpList[1 + j]);
                            value = tmp.toDouble();
                        } else {
                            value = 0;
                        }
                        YInfo yi;
                        yi.Value = value;
                        yi.IsTrusted = true;
                        m_values[i].push_back(yi);
                    }
                    _experimentTable->y().set_at(i, m_values[i]); //add it to table
                }
            }
        } else {
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
        }
        _view->updateInputs(*_experimentTable);
        if (_isFormulaModel) {

            _view->updateYY(*_experimentTable);
        }
        this->calcY(0.2);
        this->calcB(0.2);
    }
    if (MATHMODEL == _ModelType)
    {
        _view->setOptimumDesabled();
    }
    return true;
}

bool UiController::loadModel(const QString &fileName)
{
    if (fileName=="") return false;
    //_experimentTable->load(fileName.toAscii().data());

    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly))
        return false;
    QTextStream srcStream(&file);

    /* очень нужная штука для расшифровки кодировки*/
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    if (codec!= NULL)
    {
        srcStream.setCodec(codec);
    }
    else
    {
        assert(1);
    }

    int ModelType;
    srcStream >> ModelType;
    if (0 == ModelType)
    {
        _ModelType = EXTRSEARCH;
    }
    else
    {
        _ModelType = MATHMODEL;
    }

    bool r = _experimentTable->load(fileName.toAscii().data());

    if(r)
    {
        size_t factors = _experimentTable->x().count();
        std::vector<double> d, v;

        for(size_t i = 0; i < factors; i++)
        {
            d.push_back(_experimentTable->x().xDelta((int)i) * 2);
            v.push_back(_experimentTable->x().xCenter((int)i));
        }

        QString displayFunction="";
        QString cornerMetric="";
        QStringList EvalFuncData;
        EvalFuncData.append(_experimentTable->getEvaluateFunction());
        if (EvalFuncData.count()>0)
        {
            displayFunction =_experimentTable->getEvaluateFunction().at(0);
            cornerMetric = _experimentTable->getEvaluateFunction().at(1);
        }
        setDataSource(_experimentTable->x().generalFactorCount(),
                      factors, v, d,
                      _experimentTable->x().factorsDescriptions(),
                      displayFunction,
                      cornerMetric
                     );
        _interactionLevel = _experimentTable->x().getInteractionLevel();

        if ("" != _experimentTable->getEvaluateFunction().at(0)) {
            //init _experimentTable from data source dataSrc
            int replicaDelim = pow(2., _dataSrc->inputsCount()) / pow(2., _dataSrc->actualInputsCount());


                QString displayFunction1 = _dataSrc->getEvaluateFunction().at(0);
                QString conerMeasure = _dataSrc->getEvaluateFunction().at(1);
                _experimentTable = ExperimentTable::createExperimentTable(ReplicaGradient, _dataSrc->inputsCount(), replicaDelim, _interactionLevel, displayFunction1, conerMeasure);

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
            if (_isFormulaModel)
                _view->updateYY(*_experimentTable);

            return true;
        } else {

            if (EvalFuncData.count()>0)
            {
                if (""!=EvalFuncData.at(0))
                {
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
                }
            }
            _paral = _experimentTable->y().getParal();
            _interactionLevel = _experimentTable->x().getInteractionLevel();
            _modelHasData = true;
            _view->updateInputs(*_experimentTable);
            _view->updateBcoefs(*_experimentTable);
            _view->updateYY(*_experimentTable);
            //_view->updateGr(*_experimentTable);

        }

    }
    return r;
}

bool UiController::saveModel(const QString &fileName)
{
    bool saveOk = _experimentTable->save(fileName.toAscii().data());
    _view->readData(_experimentTable);
    return saveOk;
}

void UiController::calcY(double threshold)
{
    StudentChecker criteria;

    _view->readData(_experimentTable);

    _experimentTable->statisticProcessing(threshold, criteria, _evh(this, &UiController::centerYrequester),_isFormulaModel);
    _view->updateYY(*_experimentTable);
    _modelHasData = true;
}

void UiController::calcB(double threshold)
{
    _view->readData(_experimentTable);
    _experimentTable->coeffsProcessing(threshold);// threshold  ???
    _view->updateBcoefs(*_experimentTable);
    _modelHasData = true;
}

void UiController::calcExtremum(bool isMax)
{
    // CHECK CALC EXTREMUM

    _view->readData(_experimentTable);

    IExperementator *exp = new Experementator();
    bool isExtremeumFound;
    std::vector<ExperimentPoint> vals = exp->calcStepYt(isMax, *_experimentTable,
                                        _evh(this, &UiController::experimentalResponcesRequester), &isExtremeumFound);
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

bool UiController::changeModelParams()
{
    IModelSettingsView *settings =
            (IModelSettingsView*)
            new NewExperimentSettingsDialog(
                        _ModelType,
                        _dataSrc,
                        _interactionLevel,
                        _paral);

            /*_isFormulaModel ?
                                   (IModelSettingsView*)new FormulaSettingsDialog(_dataSrc, _interactionLevel) :
                                   (IModelSettingsView*)new SettingsDialog(_dataSrc, _interactionLevel);
           */
    if (settings->askNewSettings())
    {


            if (_dataSrc!=NULL)
            {
                if (settings->GetModelType()==0) // функция
                {
                    _dataSrc=NULL; // всегда обнулять датасорс для формулы
                }
            }


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
        _interactionLevel = settings->getInteractionLevel();
        _paral = settings->getParal();
        delete settings;

        //init _experimentTable from data source dataSrc
        int replicaDelim = pow(2., _dataSrc->inputsCount()) / pow(2., _dataSrc->actualInputsCount());

        if(oldInputsCount != _dataSrc->inputsCount() || oldActualInputsCount != _dataSrc->actualInputsCount()) {
            QString displayFunction = _dataSrc->getEvaluateFunction().at(0);
            QString conerMeasure = _dataSrc->getEvaluateFunction().at(1);
            _experimentTable = ExperimentTable::createExperimentTable(ReplicaGradient, _dataSrc->inputsCount(), replicaDelim, _interactionLevel, displayFunction, conerMeasure, _ModelType);

        } else {
            _experimentTable->b().changeInteractionLevel(_interactionLevel);
        }

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
        if (_isFormulaModel)
            _view->updateYY(*_experimentTable);

        return true;
    }
    return false;
}

void UiController::changeModelParamsGrad()
{
    IModelSettingsView *settings = (IModelSettingsView*) new NewExperimentSettingsDialog(this->_ModelType,_dataSrc,_interactionLevel);

            /*_isFormulaModel ?
                                   (IModelSettingsView*)new FormulaSettingsDialog(_dataSrc, _interactionLevel) :
                                   (IModelSettingsView*)new SettingsDialog(_dataSrc, _interactionLevel);
           */
    if (settings->askNewSettings())
    {

            if (_dataSrc!=NULL)
            {
                if (settings->GetModelType()==0) // функция
                {
                    _dataSrc=NULL; // всегда обнулять датасорс для формулы
                }
            }


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
        _interactionLevel = settings->getInteractionLevel();
        delete settings;

        //init _experimentTable from data source dataSrc
        int replicaDelim = pow(2., _dataSrc->inputsCount()) / pow(2., _dataSrc->actualInputsCount());

    //    if(oldInputsCount != _dataSrc->inputsCount() || oldActualInputsCount != _dataSrc->actualInputsCount())
            _experimentTable = ExperimentTable::createExperimentTable(ReplicaGradient, _dataSrc->inputsCount(), replicaDelim, _interactionLevel);
      //  else
       //     _experimentTable->b().changeInteractionLevel(_interactionLevel);

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

      //  if(!saveData)
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
        if (_isFormulaModel)
            _view->updateYY(*_experimentTable);
    }

}

void UiController::dataChanged()
{
    IExperimentTable &t = *_experimentTable;
    _view->readData(&t);

    _modelHasData = true;
    //_view->updateGr(t);
    _view->updateInputs(t);
    _view->updateYY(t);
    _view->updateBcoefs(t);
}

INumberFormatter * UiController::getNumberFormatter()
{
    if(_formatter)
        return _formatter;

    _formatter = _formatterView->getFormatter();
    return _formatter;
}

void UiController::changeNumberFormater()
{
    _formatterView->askNewFormat(_formatter);
    delete _formatter;
    _formatter = _formatterView->getFormatter();

    _view->updateBcoefs(*_experimentTable);
    //_view->updateGr(*_experimentTable);
    _view->updateInputs(*_experimentTable);
    _view->updateYY(*_experimentTable);
}

void UiController::changeFactorParams(int factorIndex)
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

void UiController::needRefresh(bool inputsOnly)
{
    _view->updateInputs(*_experimentTable);

    if(inputsOnly)
        return;

    _view->updateBcoefs(*_experimentTable);
    //_view->updateGr(*_experimentTable);
    _view->updateYY(*_experimentTable);
}

void UiController::changeBcoefSignificance(int row)
{
    const std::vector<bCoeff> &coefs = _experimentTable->b().coeffs();

    if((size_t)row >= coefs.size())
        return;

    std::vector<int> index = coefs.at(row).index();
    bool newVal = !coefs.at(row).is_significant();
    _experimentTable->b().at(index)->set_is_significant(newVal);
}


void UiController::calcAdeq()
{
    //	FisherChecker criteria;
    //    _experimentTable->y().checkAdec(criteria,_experimentTable);//   _model->calcAdeq();
    _view->updateAdeq(*_experimentTable);
}

int UiController::checkHomogeneity(StaticCriteria criteria, const char **strErr)
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

std::vector<double> UiController::gradientSearch()
{
    OptimumSearchSettingsDialog dlg;
    std::vector<double> r = dlg.askAndSearch(*_experimentTable);
    return r;
}

void UiController::calcFuncValue()
{
    CalcFuncValueDialog calcDialog;
    calcDialog.startCalcDialog(*_experimentTable);
}

void UiController::findExtr()
{
/*
    QMessageBox msg;
    msg.setText(QString::number(_experimentTable->factorCount())+'  '+QString::number(_experimentTable->b().count()));
    msg.setWindowTitle("Предупреждение");
    msg.setSizeGripEnabled(true);
    msg.exec();*/
   // for(int i = 0; i <= _experimentTable->x().count(); i++)
    int c = 0;
    for(int i = 0; i <  _experimentTable->b().count(); i++)
    {
        std::vector<bCoeff> bCoefs = _experimentTable->b().coeffs();
        std::vector<int> ind = bCoefs[i].index();
        QString s = "";
        /*
        for (int j = 0; j < ind.size(); j++)
            s = s + QString::number(ind[j]) + ' ';
            */
        for(int j = 0; j < (int)ind.size(); j++)
        {
            s += QString::number(ind[j]);
            //str+=" ";
        }


        for(int k = 0; k <  _experimentTable->x().count(); k++)
        {
            QString s2 = QString::number(k);
            if(s==s2)
                c++;
        }


    }
/*
    if(c == _experimentTable->x().count())
    {
        QMessageBox msg;
        msg.setText("все линейные посчитаны");
        msg.setWindowTitle("Предупреждение");
        msg.setSizeGripEnabled(true);
        msg.exec();
    }*/
    if(c == _experimentTable->x().count())
    {
        //для проверки это формульный эксперимент или нет
        std::vector<int> xxx;
        for(int i = 0; i < _experimentTable->x().count(); i++)
        {
            xxx.push_back(1);
        }

        std::vector<double> yyy = _dataSrc->getYdata(xxx);

        findExtrDialog CfindExtrDialog;

        //если не производится расчет по формуле, то это не формульный эксперимент
        if(yyy.size()==0)
            CfindExtrDialog.isFormulaExperiment = false;
        else
           CfindExtrDialog.isFormulaExperiment = true;

        CfindExtrDialog.startfindExtr(*_experimentTable, *_dataSrc);


        IExperimentTable * exTable = NULL;
        exTable = ExperimentTable::createExperimentTable(CentralOrtogonalComposite, _dataSrc->inputsCount(), _dataSrc->actualInputsCount(), _interactionLevel);
        ExperimentDialog * exDialog = new ExperimentDialog();

       // CHECK CALC EXTREMUM

        _view->readData(_experimentTable);

        std::vector<ExperimentPoint> vals = CfindExtrDialog.v;

        bool isExtremeumFound = CfindExtrDialog.isExtremFound;

        //assert(vals.at(vals.size() - 1).xs.size() == (size_t)_experimentTable->x().count()); // xs[0] is f

        if((CfindExtrDialog.ccp==true)&&(CfindExtrDialog.exper==false))
        {
            _view->updateGr(*_experimentTable, vals); // + isExtremeumFound

            exDialog->startDialog("ЦКП", exTable, _experimentTable, _interactionLevel);
        }



        std::vector<ExperimentPoint> vals2 = vals;
        vals.clear();
        for(int i = 0; i < vals2.size(); i++)
        {
           ExperimentPoint P = vals2[i];
           std::vector<double> va;
           for(int j = 0; j < P.xs.size(); j++)
           {
             //  remove(P.xs.begin(), P.xs.end(), 0 );
               //if(P.xs[j]!=0)
               std::vector<bCoeff> bCoefs = _experimentTable->b().coeffs();
               if (bCoefs[j+1].is_significant())
               {
                   va.push_back(P.xs[j]);
               }
           }
           P.xs.clear();
           P.xs = va;
           vals.clear();
           vals.push_back(P);
        }

        if ((CfindExtrDialog.exper == true)&&(CfindExtrDialog.ccp == false))
        {
            _modelHasData = true;

            int pos=0;
            int xspos=0;
            _view->updateGr(*_experimentTable, vals);

            int countSign = 0;

            int countIsSign = 0;
            /*
            for(int i = 0; i < _experimentTable->x().count(); i++)
            {
                std::vector<bCoeff> bCoefs = _experimentTable->b().coeffs();
                if (bCoefs[i+1].is_significant())
                {
                    countIsSign++;
                }
             }

            for(int i = 0; i < ((ExperimentPoint)vals[i]).xs.size(); i++)
            {
                std::vector<bCoeff> bCoefs = _experimentTable->b().coeffs();
                //if (((ExperimentPoint)vals.at(0)).xs.at(xspos)!= 0)
                if (bCoefs[i+1].is_significant())
                {
                    _experimentTable->x().setXcenter(pos, vals.at(vals.size() - 1).xs.at(xspos));
                    pos++;
                }
                else
                {
                    countSign++;
                }
                xspos++;

            }
*/
            ResponcesSourseFunction *src = new ResponcesSourseFunction((ResponcesSourseFunction&)*_dataSrc);
            std::vector<double> v;

            src->FactNum = (((ExperimentPoint)vals[0]).xs.size() - countSign);
            src->ActualFactNum = (((ExperimentPoint)vals[0]).xs.size() - countSign);
            std::vector<double> _FactDivergences;
           /* _FactDivergences.push_back(0.5);
            _FactDivergences.push_back(0.5);
            _FactDivergences.push_back(0.5);*/
            for(int i = 0; i < ((ExperimentPoint)vals[0]).xs.size(); i++)
            {
                _FactDivergences.push_back(CfindExtrDialog._FactDivergences);
            }
            src->FactDivergences = _FactDivergences;



            for(int i = 0; i < src->FactNum; i++)
                v.push_back(_experimentTable->x().xCenter((int)i));

            src->FactValues = vals.at(vals.size() - 1).xs;
            _dataSrc = src;
            _view->updateInputs(*_experimentTable);

            this->newModellGrad(CfindExtrDialog.isFormulaExperiment);
        }
    }
    else
    {
        QMessageBox msg;
        msg.setText("Найдены не все линейные коэффициенты");
        msg.setWindowTitle("Предупреждение");
        msg.setSizeGripEnabled(true);
        msg.exec();
    }
}

//called when center Ys are needed.
std::vector<double> UiController::centerYrequester(const std::vector<double> &centerVals)
{
    ICenterExperimentsView *view = new CenterExperimentsDialog();

    std::vector<double> yc;
    view->askCenterYs(yc);
    yc = view->getCenterYs();
    delete view;

    return yc;
}

std::vector<ExperimentPoint> UiController::experimentalResponcesRequester(const std::vector<ExperimentPoint> &centerVals)
{
    assert((size_t)_experimentTable->x().count() == centerVals.at(centerVals.size() - 1).xs.size());
    std::vector<int> factsNums(_experimentTable->x().count());

    for(size_t i = 0; i < factsNums.size(); i++)
        factsNums[i] = i + 1;

    ImaginarySteps stepsDialog(factsNums, centerVals); //<vector<vector<>> + ExperimentPoints
    std::vector<ExperimentPoint> retPts = stepsDialog.getYp();
    return retPts;
}

std::vector<double> UiController::getProbsList(StaticCriteria criteria)
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

void UiController::initEmptyModel()
{
    int factors = 3, baseFactors = 2;
    _experimentTable = ExperimentTable::createExperimentTable(ReplicaGradient, factors, baseFactors, _interactionLevel);

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


void UiController::initEmptyModelGrad()
{
    int factors = 3, baseFactors = 2;
    _experimentTable = ExperimentTable::createExperimentTable(ReplicaGradient, factors, baseFactors, _interactionLevel);

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
void UiController::setDataSource(int baseFactors,
                                 int factorsCount,
                                 const std::vector<double> &centers,
                                 const std::vector<double> &deltas,
                                 QStringList descriptions,
                                 QString displeyFunction,
                                 QString cornerMeasure)
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
    if (displeyFunction=="")
    {
        src->SetEvaluateFunction("","");
    }
    else
    {
        QString tmpstr=displeyFunction;
        if ("degrees" == cornerMeasure)
        {
            tmpstr=src->CheckFormulaForTrinometricFunctions(tmpstr);
        }
        src->SetEvaluateFunction(displeyFunction,tmpstr);
    }
    src->parall=0;

    src->DrobRepl=2./3;
    _dataSrc = src;
}


IExperimentTable * UiController::getExperimentTable()
{
    return _experimentTable;
}

void UiController::startProcess(bool max, double _h, int _ch, int _dev)
{
    IExperimentTable * exTable = NULL;
    exTable = ExperimentTable::createExperimentTable(CentralOrtogonalComposite, _dataSrc->inputsCount(), _dataSrc->actualInputsCount(), _interactionLevel);
    ExperimentDialog * exDialog = new ExperimentDialog();

    bool isMax = max;

    // CHECK CALC EXTREMUM

    _view->readData(_experimentTable);

    //IExperementator *exp=new Experementator();
    IExperementator *exp = new Experementator(_h, _ch, _dev);
    bool isExtremeumFound;
    std::vector<ExperimentPoint> vals = exp->calcStepYt(isMax, *_experimentTable,
                                        _evh(this, &UiController::experimentalResponcesRequester), &isExtremeumFound);
    delete exp;

    assert(vals.at(vals.size() - 1).xs.size() == (size_t)_experimentTable->x().count()); // xs[0] is f

    if(isExtremeumFound)
    {
        _view->updateGr(*_experimentTable, vals); // + isExtremeumFound

        exDialog->startDialog("ЦКП", exTable, _experimentTable, _interactionLevel);

        OptimumSearchSettingsDialog dlg;
        std::vector<double> r = dlg.askAndSearch(*_experimentTable, vals.at(vals.size() - 1));
        QString s;

        for(size_t i = 0; i < r.size(); i++)
            s.append(QString::number(r[i]) + ", ");

        s.chop(2);// last ", "
        QMessageBox msg;
        msg.setText("X={" + s + "}");
        msg.setWindowTitle("Вектор-результат градиентного поиска");
        msg.setSizeGripEnabled(true);
        msg.exec();

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

void UiController::CCP()
{
    IExperimentTable * exTable = NULL;
    exTable = ExperimentTable::createExperimentTable(CentralOrtogonalComposite, _dataSrc->inputsCount(), _dataSrc->actualInputsCount(), _interactionLevel);
    ExperimentDialog * exDialog = new ExperimentDialog();
    exDialog->startDialog("ЦКП", exTable, _experimentTable, _interactionLevel);
}

void UiController::ChSaveFolderPath()
{
    SaveDirectoryPathDialog * saveDialog = new SaveDirectoryPathDialog();
    saveDialog->show();

}
