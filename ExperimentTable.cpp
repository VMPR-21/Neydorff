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

IFactorTable& ExperimentTable::x() const
{
    assert(_xTable != NULL);
    return *_xTable;
}

IResponseTable& ExperimentTable::y() const
{
    assert(_yTable != NULL);
    return *_yTable;
}

IRegressionCoefficientTable& ExperimentTable::b() const
{
    assert(_bTable != NULL);
    return *_bTable;
}

int ExperimentTable::factorCount() const
{
    assert(_xTable != NULL);
    return _xTable->count();
}

int ExperimentTable::rowCount() const
{
    assert(_xTable != NULL);
    return _xTable->rowCount();
}

void ExperimentTable::statisticProcessing(double trust_probability, ICriteria &criteria, CenterYDelegatePtr ysRequestDel, bool isFormulaModel)
{
    _yTable->statProcessing(criteria, trust_probability, ysRequestDel,isFormulaModel);
}

void ExperimentTable::coeffsProcessing(double trust_probability)
{
    _bTable->calculate(*_xTable, *_yTable, trust_probability);
}
/*
double CalculateBX(const bCoeff &b, const std::vector<double> xvals)
{
    if(b.index()[0] == 0)   // b0
        return b.natural_value();
    else
    {
        double result = b.natural_value();

        for(size_t xNumber = 0; xNumber < b.index().size(); xNumber++)
            result *= xvals[b.index()[xNumber] - 1];//result *= xvals[xNumber - 1];

        return result;
    }
}

double ExperimentTable::evalFuncValue(const std::vector<double> xvals) const
{
    int _bTableCount = _bTable->count();
    int xvalsCount = xvals.size();
    int _xTableCount = _xTable->count();

    int iiii = 0;

    assert(_xTable != NULL && _yTable != NULL && _bTable != NULL);
    assert(xvals.size() == (size_t)_xTable->count());

    double result = 0.0;

    for(int bNumber = 0; bNumber < _bTable->count(); bNumber++)
        result += CalculateBX(_bTable->coeffs()[bNumber], xvals);

    return result;
}
*/
double CalculateBX(const bCoeff &b, const std::vector<double> xvals)
{
    if(b.index()[0] == 0)   // b0
        return b.natural_value();
    else
    {
        double result = b.natural_value();

        for(size_t xNumber = 0; xNumber < b.index().size(); xNumber++)
            result *= xvals[b.index()[xNumber] - 1];//result *= xvals[xNumber - 1];

        return result;
    }
}

double ExperimentTable::evalFuncValue(const std::vector<double> xvals) const
{
    //assert(_xTable != NULL && _yTable != NULL && _bTable != NULL);
    //assert(xvals.size() == (size_t)_xTable->count());

    double result = 0.0;

    for(int bNumber = 0; bNumber < _bTable->count(); bNumber++)
        result += CalculateBX(_bTable->coeffs()[bNumber], xvals);

    return result;
}

ExperimentTable::~ExperimentTable()
{
    delete _xTable;
    delete _yTable;
    delete _bTable;
}

static const quint8 CURRENT_FILE_FORMAT_VER = 1;//increment, when you change save format

bool ExperimentTable::load(const char *fileName)
{
    QFile file(fileName);

    if(!file.open(QIODevice::ReadOnly))
        return false;


    string fileExtension = string(fileName);
    fileExtension = fileExtension.substr(fileExtension.find_last_of(".")+1);
    transform(fileExtension.begin(),fileExtension.end(),fileExtension.begin(),(int(*)(int))::tolower);
    if (!fileExtension.compare("csv"))
    {
        QTextStream stream(&file);
        this->loadEvaluateFunctionFromCSV(stream);
        _xTable->loadFromCSV(stream);
        this->loadExtrFromCSV(stream);
        _yTable->loadFromCSV(stream);
        _bTable->loadFromCSV(stream);
    }
    else
    {
        DataStream stream(&file);
        quint8 v;
            stream >> v;

        if(v != CURRENT_FILE_FORMAT_VER)
            return false;
        this->loadExtr(stream);
        this->loadEvaluateFunction(stream);
        _xTable->loadFrom(stream);
        _yTable->loadFrom(stream);
        _bTable->loadFrom(stream);
    }

    return true;
}

bool ExperimentTable::save(const char *fileName)
{
    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly))
        return false;

    // а вот здесь нужно получить из имени файла его расширение
    // и если оно равно "csv", то сохраняем в csv
    string fileExtension = string(fileName);
    fileExtension = fileExtension.substr(fileExtension.find_last_of(".")+1);
    transform(fileExtension.begin(),fileExtension.end(),fileExtension.begin(),(int(*)(int))::tolower);
    if (!fileExtension.compare("csv")) {
        QTextStream stream(&file);
        QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
        if (codec!= NULL)
        {
            stream.setCodec(codec);
        }
        else
        {
            assert(1);
        }
        //stream << CURRENT_FILE_FORMAT_VER;
        this->saveEvaluateFunctionToCSV(stream);
        _xTable->saveToCSV(stream);
        this->saveExtrToCSV(stream);
        _yTable->saveToCSV(stream);
        _bTable->saveToCSV(stream);
        this->saveExperimentPointToCSV(stream);
    } else {
        DataStream stream(&file);
        stream << CURRENT_FILE_FORMAT_VER;
        this->saveExtr(stream);
        this->saveEvaluateFunction(stream);
        _xTable->saveTo(stream);
        _yTable->saveTo(stream);
        _bTable->saveTo(stream);
        this->saveExperimentPoint(stream);
    }
    return true;
}

bool ExperimentTable::save(const char *fileName) const
{
    QFile file(fileName);

    if(!file.open(QIODevice::WriteOnly))
        return false;

    // а вот здесь нужно получить из имени файла его расширение
    // и если оно равно "csv", то сохраняем в csv
    string fileExtension = string(fileName);
    fileExtension = fileExtension.substr(fileExtension.find_last_of(".")+1);
    transform(fileExtension.begin(),fileExtension.end(),fileExtension.begin(),(int(*)(int))::tolower);
    if (!fileExtension.compare("csv")) {
        QTextStream stream(&file);
        QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
        if (codec!= NULL)
        {
            stream.setCodec(codec);
        }
        else
        {
            assert(1);
        }
        //stream << CURRENT_FILE_FORMAT_VER;
        this->saveEvaluateFunctionToCSV(stream);
        _xTable->saveToCSV(stream);
        this->saveExtrToCSV(stream);
        _yTable->saveToCSV(stream);
        _bTable->saveToCSV(stream);
        this->saveExperimentPointToCSV(stream);
    } else {
        DataStream stream(&file);
        stream << CURRENT_FILE_FORMAT_VER;
        this->saveExtr(stream);
        this->saveEvaluateFunction(stream);
        _xTable->saveTo(stream);
        _yTable->saveTo(stream);
        _bTable->saveTo(stream);
        this->saveExperimentPoint(stream);
    }
    return true;
}

//ExperimentTable::ExperimentTable(int factor_count, int row_count, int replica_delimiter)
//{
//TODO: FIX. ERROR!!! virtual func. call from .ctor() !!
//_xTable = CreateFactorTable(factor_count, row_count, replica_delimiter);
//_yTable = CreateResponseTable(row_count);
//_bTable = CreateRegressionCoeffsTable();
//}

ExperimentTable::ExperimentTable()
{
}

int calculateGradTableRowCount(int factor_count, int replica_delimiter)
{
    assert(factor_count > 1);
    assert(replica_delimiter > 0);

    return static_cast<int>(pow(2.0, factor_count)) / replica_delimiter;
}

int calculateCOCPTableRowCount(int factor_count)
{
    return static_cast<int>(pow(2.0, factor_count)) + 2 * factor_count + 1;  // кол-во опытов в эксперименте
}

IExperimentTable* ExperimentTable::createGradientExperimentTable(int factor_count, int replica_delimiter, int interaction_level, QString displeyFunction, QString cornerMeasure, short ModelType)
{
    ExperimentTable *expTable = new ExperimentTable();

    int row_count = calculateGradTableRowCount(factor_count, replica_delimiter);
    expTable->_xTable = new GradientFactorTable(factor_count, row_count, replica_delimiter);
    expTable->_xTable->setInteractionLevel(interaction_level);
    expTable->_yTable = new ResponseTable(row_count);
    //expTable->_bTable = new GradientRegressionCoefficientTable(factor_count, 1);
    expTable->_bTable =  RegressionCoefficientTable::Create(FullFactorGradient, factor_count, interaction_level);
    expTable->evaluateFunction.clear();
    expTable->evaluateFunction << displeyFunction << cornerMeasure;

    // От Коли
    expTable->isMax = true;
    expTable->strideParameter = 0.50;
    expTable->numberStride = 7;
    expTable->interestAllowedDeviation = 30;
    // От Коли
    expTable->_ModelType = ModelType;

    return expTable;
}

IExperimentTable* ExperimentTable::createCentralCompositeExperimentTable(int factor_count, int interaction_level, QString displeyFunction, QString cornerMeasure, short ModelType)
{
    ExperimentTable *expTable = new ExperimentTable();

    int row_count = calculateCOCPTableRowCount(factor_count);
    expTable->_xTable = new CentralOrtogonalFactorTable(factor_count, row_count);
    expTable->_xTable->setInteractionLevel(interaction_level);
    expTable->_yTable = new ResponseTable(row_count);
    //expTable->_bTable = new CentralOrtogonalRegressionCoefficientTable(factor_count, 1);
    expTable->_bTable = RegressionCoefficientTable::Create(CentralOrtogonalComposite, factor_count, interaction_level);
    expTable->evaluateFunction.clear();
    expTable->evaluateFunction << displeyFunction << cornerMeasure;

    // От Коли
    expTable->isMax = true;
    expTable->strideParameter = 0.50;
    expTable->numberStride = 7;
    expTable->interestAllowedDeviation = 30;
    // От Коли
    expTable->_ModelType = ModelType;

    return expTable;
}

IExperimentTable* ExperimentTable::createExperimentTable(TableType tableType, int factor_count, int replica_delimiter, int interaction_level, QString displeyFunction, QString cornerMeasure, short ModelType)
{
    assert(factor_count > 1);
    assert(replica_delimiter > 0);

    switch(tableType)
    {
    case FullFactorGradient:
        return createGradientExperimentTable(factor_count, 1, interaction_level, displeyFunction, cornerMeasure, ModelType);
        break;

    case ReplicaGradient:
        return createGradientExperimentTable(factor_count, replica_delimiter, interaction_level, displeyFunction, cornerMeasure, ModelType);

    case CentralOrtogonalComposite:
        return createCentralCompositeExperimentTable(factor_count, interaction_level, displeyFunction, cornerMeasure, ModelType);

    default:
        assert(0 != 0); // неподдерживаемый или неизвестный тип таблицы
    }

    return NULL;
}

QString ExperimentTable::doubleWithComma(double d) {
   return QString::number(d).replace(".",",");
}
QString ExperimentTable::doubleWithDot(QString d) {
   return d.replace(",",".");
}

void ExperimentTable::saveEvaluateFunction(DataStream &srcStream)
{
    if ("" != this->evaluateFunction.at(0)) {
        srcStream  << 1; // будет записывать 1 если есть фонмула
        srcStream  << this->evaluateFunction.at(0);
        if (this->evaluateFunction.at(1).contains("/*degrees*/")) {
            srcStream << "degrees";
        } else {
            srcStream << "radian";
        }
    } else {
        srcStream  << (quint64)0; // будет записывать 0 если нет фонмулы
    }
}

void ExperimentTable::saveEvaluateFunction(DataStream &srcStream) const
{
    if ("" != this->evaluateFunction.at(0)) {
        srcStream  << 1; // будет записывать 1 если есть фонмула
        srcStream  << this->evaluateFunction.at(0);
        if (this->evaluateFunction.at(1).contains("/*degrees*/")) {
            srcStream << "degrees";
        } else {
            srcStream << "radian";
        }
    } else {
        srcStream  << (quint64)0; // будет записывать 0 если нет фонмулы
    }
}

void ExperimentTable::saveEvaluateFunctionToCSV(QTextStream &srcStream)
{
    if ("" != this->evaluateFunction.at(0)) {
        srcStream << QString::fromUtf8("Функция:") << ";" << this->evaluateFunction.at(0) << ";\r\n";
        if (this->evaluateFunction.at(1).contains("/*degrees*/")) {
            srcStream << QString::fromUtf8("Мера угла:") << ";" << "degrees" << ";\r\n";
        } else {
            srcStream << QString::fromUtf8("Мера угла:") << ";" << "radian" << ";\r\n";
        }
    }
}

void ExperimentTable::saveEvaluateFunctionToCSV(QTextStream &srcStream) const
{
    if ("" != this->evaluateFunction.at(0)) {
        srcStream << QString::fromUtf8("Функция:") << ";" << this->evaluateFunction.at(0) << ";\r\n";
        if (this->evaluateFunction.at(1).contains("/*degrees*/")) {
            srcStream << QString::fromUtf8("Мера угла:") << ";" << "degrees" << ";\r\n";
        } else {
            srcStream << QString::fromUtf8("Мера угла:") << ";" << "radian" << ";\r\n";
        }
    }
}
void ExperimentTable::loadEvaluateFunction(DataStream &srcStream){
    QString function = "", measure = "";
    quint64 existFormula;
    srcStream >> existFormula;
    if (1 == existFormula) {
        srcStream >> function;
        srcStream >> measure;
        this->evaluateFunction << function << measure;
    } else {
        this->evaluateFunction << "" << "";
    }
}
void ExperimentTable::loadEvaluateFunctionFromCSV(QTextStream &srcStream)
{
    this->evaluateFunction.clear();
    QString tmp_str = srcStream.readLine();
    QString function = tmp_str.split(QRegExp(";"))[1];
    if ("" != function) {
        QString measure = srcStream.readLine().split(QRegExp(";"))[1];
        this->evaluateFunction << function << measure;
    } else {
        this->evaluateFunction << "" << "";
    }
}
void ExperimentTable::setEvaluateFunction(QString evaluate, QString measure)
{
    this->evaluateFunction << evaluate << measure;
}

QStringList ExperimentTable::getEvaluateFunction()
{
    return this->evaluateFunction;
}
// От Коли
bool ExperimentTable::getIsMax() const
{ return this->isMax;}
double ExperimentTable::getStrideParameter() const
{ return this->strideParameter;}
int ExperimentTable::getNumberStride() const
{ return this->numberStride;}
double ExperimentTable::getInterestAllowedDeviation() const
{ return this->interestAllowedDeviation;}

void ExperimentTable::setIsMax(bool isMax) const
{ this->isMax = isMax;}
void ExperimentTable::setStrideParameter(double strideParameter) const
{ this->strideParameter = strideParameter;}
void ExperimentTable::setNumberStride(int numberStride) const
{ this->numberStride = numberStride;}
void ExperimentTable::setInterestAllowedDeviation(double interestAllowedDeviation) const
{ this->interestAllowedDeviation = interestAllowedDeviation;}

void ExperimentTable::saveExtr(DataStream &srcStream)
{
    if (MATHMODEL == this->_ModelType)
    {
        srcStream << 1; // Если мат. модель
    }
    else
    {
        srcStream << 0; // Если поиск оптимума
        srcStream << this->isMax;
        srcStream << this->strideParameter;
        srcStream << this->numberStride;
        srcStream << this->interestAllowedDeviation;
    }
}

void ExperimentTable::saveExtr(DataStream &srcStream) const
{
    if (MATHMODEL == this->_ModelType)
    {
        srcStream << 1; // Если мат. модель
    }
    else
    {
        srcStream << 0; // Если поиск оптимума
        srcStream << this->isMax;
        srcStream << this->strideParameter;
        srcStream << this->numberStride;
        srcStream << this->interestAllowedDeviation;
    }
}

void ExperimentTable::loadExtr(DataStream &srcStream)
{
    int ModelType;
    srcStream >> ModelType;
    if (0 == ModelType)
    {
        srcStream >> this->isMax;
        srcStream >> this->strideParameter;
        srcStream >> this->numberStride;
        srcStream >> this->interestAllowedDeviation;
    }
}

void ExperimentTable::saveExtrToCSV(QTextStream &srcStream)
{
    if (MATHMODEL == this->_ModelType)
    {
        srcStream << QString::fromUtf8("Тип модели:") << ";" << "MATHMODEL" << ";\r\n";
    }
    else
    {
        srcStream << QString::fromUtf8("Тип модели:") << ";" << "OPTIMUM" << ";\r\n";
        srcStream << QString::fromUtf8("Поиск точек перегиба") << ";\r\n";
        srcStream << QString::fromUtf8("По максимуму:") << ";" << this->isMax << ";\r\n";
        srcStream << QString::fromUtf8("Величина шага:") << ";" << ExperimentTable::doubleWithComma(this->strideParameter) << ";\r\n";
        srcStream << QString::fromUtf8("Количество шагов:") << ";" << this->numberStride << ";\r\n";
        srcStream << QString::fromUtf8("Допустимое отклонение:") << ";" << ExperimentTable::doubleWithComma(this->interestAllowedDeviation) << ";\r\n";
    }
}

void ExperimentTable::saveExtrToCSV(QTextStream &srcStream) const
{
    if (MATHMODEL == this->_ModelType)
    {
        srcStream << QString::fromUtf8("Тип модели:") << ";" << "MATHMODEL" << ";\r\n";
    }
    else
    {
        srcStream << QString::fromUtf8("Тип модели:") << ";" << "OPTIMUM" << ";\r\n";
        srcStream << QString::fromUtf8("Поиск точек перегиба") << ";\r\n";
        srcStream << QString::fromUtf8("По максимуму:") << ";" << this->isMax << ";\r\n";
        srcStream << QString::fromUtf8("Величина шага:") << ";" << ExperimentTable::doubleWithComma(this->strideParameter) << ";\r\n";
        srcStream << QString::fromUtf8("Количество шагов:") << ";" << this->numberStride << ";\r\n";
        srcStream << QString::fromUtf8("Допустимое отклонение:") << ";" << ExperimentTable::doubleWithComma(this->interestAllowedDeviation) << ";\r\n";
    }
}

void ExperimentTable::loadExtrFromCSV(QTextStream &srcStream)
{
    srcStream.readLine();
    QString tmp = srcStream.readLine();
    tmp = tmp.split(QRegExp(";"))[1];
    this->isMax = tmp.toInt();
    tmp = srcStream.readLine();
    tmp = ExperimentTable::doubleWithDot(tmp.split(QRegExp(";"))[1]);
    this->strideParameter = tmp.toDouble();
    tmp = srcStream.readLine();
    tmp = tmp.split(QRegExp(";"))[1];
    this->numberStride = tmp.toInt();
    tmp = srcStream.readLine();
    tmp = ExperimentTable::doubleWithDot(tmp.split(QRegExp(";"))[1]);
    this->interestAllowedDeviation = tmp.toDouble();
}

void ExperimentTable::setExperimentPoint(std::vector<ExperimentPoint> v) const
{
    this->v = v;
}

void ExperimentTable::saveExperimentPointToCSV(QTextStream &srcStream)
{
    if (0 < this->v.size())
    {
        ExperimentPoint aught = v[0];
        srcStream << QString::fromUtf8("ГРАДИЕНТ") << ";";
        for (int i = 0; i < aught.xs.size(); i++) {
            srcStream << QString::fromUtf8("x") << i << ";";
        }
        srcStream << QString::fromUtf8("Y grad") << ";";
        srcStream << QString::fromUtf8("Y real") << ";\r\n";

        for (int i = 0; i < this->v.size(); i++)
        {
            aught = v.at(i);
            srcStream << QString::fromUtf8("Шаг ") << i+1 << ";";
            for (int j = 0; j < aught.xs.size(); j++) {
                srcStream << ExperimentTable::doubleWithComma(aught.xs[j]) << ";";
            }
            srcStream << ExperimentTable::doubleWithComma(aught.yt) << ';';
            srcStream << ExperimentTable::doubleWithComma(aught.yp) << ";\r\n";
        }
    }
}

void ExperimentTable::saveExperimentPointToCSV(QTextStream &srcStream) const
{
    if (0 < this->v.size())
    {
        ExperimentPoint aught = v[0];
        srcStream << QString::fromUtf8("ГРАДИЕНТ") << ";";
        for (int i = 0; i < aught.xs.size(); i++) {
            srcStream << QString::fromUtf8("x") << i << ";";
        }
        srcStream << QString::fromUtf8("Y grad") << ";";
        srcStream << QString::fromUtf8("Y real") << ";\r\n";

        for (int i = 0; i < this->v.size(); i++)
        {
            aught = v.at(i);
            srcStream << QString::fromUtf8("Шаг ") << i+1 << ";";
            for (int j = 0; j < aught.xs.size(); j++) {
                srcStream << ExperimentTable::doubleWithComma(aught.xs[j]) << ";";
            }
            srcStream << ExperimentTable::doubleWithComma(aught.yt) << ';';
            srcStream << ExperimentTable::doubleWithComma(aught.yp) << ";\r\n";
        }
    }
}

void ExperimentTable::saveExperimentPoint(DataStream &srcStream)
{
    if (0 < this->v.size())
    {
        ExperimentPoint aught = v[0];
        for (int i = 0; i < this->v.size(); i++)
        {
            aught = v.at(i);
            for (int j = 0; j < aught.xs.size(); j++) {
                srcStream << aught.xs[j];
            }
            srcStream << aught.yt;
            srcStream << aught.yp;
        }
    }
}

void ExperimentTable::saveExperimentPoint(DataStream &srcStream) const
{
    if (0 < this->v.size())
    {
        ExperimentPoint aught = v[0];
        for (int i = 0; i < this->v.size(); i++)
        {
            aught = v.at(i);
            for (int j = 0; j < aught.xs.size(); j++) {
                srcStream << aught.xs[j];
            }
            srcStream << aught.yt;
            srcStream << aught.yp;
        }
    }
}
void ExperimentTable::setModelType(short ModelType)
{
    this->_ModelType = ModelType;
}
short ExperimentTable::getModelType()
{
    return this->_ModelType;
}
