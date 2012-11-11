
#include "FactorTable.h"
#include "assert.h"
#include <vector>
#include <limits>
#include "ExperimentTable.h"
#include <QTextCodec>
using namespace std;

int FactorTable::count() const
{
    assert(_x != NULL);
    return _x->at(0)->size();
}


int FactorTable::rowCount() const
{
    assert(_x != NULL);
    return _x->size();
}

double FactorTable::norm_at(int number, int row) const
{
    assert(row >= 0 && (size_t)row < _x->size());
    assert(number >= 0 && (size_t)number < _x->at(0)->size());
    return _x->at(row)->at(number);
}

double FactorTable::natural_at(int number, int row) const
{
    assert(number >= 0 && number < count());
    assert(row >= 0 && row < rowCount());
    return xCenter(number) + norm_at(number, row) * xDelta(number);
}

double FactorTable::xDelta(int number) const
{
    assert(number >= 0 && number < count());
    return _delta->at(number);
}

void FactorTable::setXdelta(int number, double d)
{
    // TODO: проверять d на NaN и +-Infinity
    assert(number >= 0 && number < count());
    _delta->at(number) = d;
}

double FactorTable::xCenter(int number) const
{
    assert(number >= 0 && number < count());
    return _center->at(number);
}

void FactorTable::setXcenter(int number, double c)
{
    // TODO: проверять c на NaN и +-Infinity
    assert(number >= 0 && number < count());
    _center->at(number) = c;
}

void FactorTable::Remove(int Xnumber)
{
    if (_center->size()>0)
        _center->erase(_center->begin()+Xnumber);
    if (_descriptions.size()>0)
        _descriptions.erase(_descriptions.begin()+Xnumber);
    if (_delta->size()>0)
        _delta->erase(_delta->begin()+Xnumber);
}

QStringList FactorTable::factorsDescriptions() const
{
    return _descriptions;
}

void FactorTable::setFactorsDescriptions(const QStringList &descriptions)
{
    _descriptions = descriptions;
}

FactorTable::FactorTable(int factor_count, int row_count)
{
    assert(factor_count > 1);
    assert(row_count > 1);

    // создание таблицы нормированных значений факторов
    _x = new vector<vector<double>* >(row_count);

    for(int row = 0; row < row_count; row++)
    {
        _x->at(row) = new vector<double>(factor_count);

        for(int factor = 0; factor < factor_count; factor++)
            _x->at(row)->at(factor) = std::numeric_limits<double>::quiet_NaN();
    }

    // создание центров и интервалов варьирования факторов
    _center = new vector<double>(factor_count);
    _delta = new vector<double>(factor_count);
}

FactorTable::~FactorTable()
{
    assert(_x != NULL);
    assert(_center != NULL);
    assert(_delta != NULL);

    for(size_t row = 0; row < _x->size(); row++)
        delete _x->at(row);

    delete _x;
    delete _center;
    delete _delta;
}

void FactorTable::loadFrom(DataStream &srcStream)
{
    quint64 factor_count, row_count;
    double num;
    srcStream >> row_count;
    _x = new vector<vector<double>* >(row_count);

    for(size_t row = 0; row < row_count; row++)
    {
        srcStream >> factor_count;
        _x->at(row) = new vector<double>(factor_count);

        for(size_t factor = 0; factor < factor_count; factor++)
        {
            srcStream >> num;
            _x->at(row)->at(factor) = num;
        }
    }

    _center = new std::vector<double>(factor_count);
    _delta = new std::vector<double>(factor_count);

    for(size_t i = 0; i < factor_count; i++)
    {
        srcStream >> num;
        _center->at(i) = num;
        srcStream >> num;
        _delta->at(i) = num;
    }

    _descriptions.clear();
    quint64 descrCount;
    srcStream >> descrCount;

    for(size_t i = 0; i < descrCount; i++)
    {
        QString d;
        srcStream >> d;
        _descriptions << d;
    }
}

void FactorTable::loadFromCSV(QTextStream &srcStream)
{

}

void FactorTable::saveTo(DataStream &dstStream)
{
    dstStream << (quint64)_x->size();

    for(size_t row = 0; row < _x->size(); row++)
    {
        dstStream << (quint64)_x->at(row)->size();

        for(size_t factor = 0; factor < _x->at(row)->size(); factor++)
            dstStream << _x->at(row)->at(factor);
    }

    for(size_t i = 0; i < _center->size(); i++)
    {
        dstStream << _center->at(i);
        dstStream << _delta->at(i);
    }

    quint64 sz = (quint64)_descriptions.size();
    dstStream << sz;

    for(size_t i = 0; i < sz; i++)
        dstStream << _descriptions[i];
}

void FactorTable::saveToCSV(QTextStream &dstStream)
{

}

void FactorTable::set_at(int factor, int row, double value)
{
    assert(_x != NULL);
    _x->at(row)->at(factor) = value;
}

int FactorTable::getInteractionLevel()
{
    return this->_interactionLevel;
}

void FactorTable::setInteractionLevel(int interactionLevel)
{
    this->_interactionLevel = interactionLevel;
}
