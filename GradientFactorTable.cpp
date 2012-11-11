#include "GradientFactorTable.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <QTextCodec>
#include "ExperimentTable.h"
#include <limits>

#ifdef _MSC_VER
static double log2(double n) //log2() is not supported by MSVC
{
    // log(n)/log(2) is log2.
    return log( n ) / log( 2. );
}
#endif

using namespace std;


// функция для проверки, является ли сгенерированный индекс максимальным
static bool index_is_max(std::vector<int> index, int max_digits, int max_value)
{
    if((int)index.size() < max_digits) return false;

    for(int i = 0; i < (int)index.size(); i++)
        if(index[i] < max_value - ((int)index.size() - i - 1))
            return false;

    return true;
}

//получение следующей комбинаци
//vec - целевой вектор с значениеми перестановки
//maxDigits - макс. допустимое число разрядов в комбинации
//maxVal - макс. значение каждого разряда
//minVal - мин. значение каждого раздяда.
template<typename T> static std::vector<T> nextVec(const std::vector<T> &vec, int maxDigits = 256, int maxVal = 9, int minVal = 1)
{
    int sz = vec.size();
    int i = sz - 1;
    std::vector<T> res;

    if(index_is_max(vec, maxDigits, maxVal))
        return res;//no more permutations

    //find digit for increment
    while(i >= 0 && vec.at(i) >= (maxVal - sz) + i + 1 )
        i--;

    if(i < 0)
    {
        if(sz >= maxDigits)
            return res;//we reached our maximum

        //increase size and "reset"
        res.resize(sz + 1, minVal);

        for(int i = 0; i < sz + 1; i++)
            res[i] = minVal + i;
    }
    else
    {
        res = vec;
        //increase digit
        res[i]++;

        //change the rest of items
        for(int j = i + 1; j < sz; j++)
            res[j] = res[j - 1] + 1;
    }

    return res;
}

// проверка, является ли число степенью 2
bool ispow2(int value)
{
    if(value <= 0) return false;    // допустимы лишь положительные значения

    if(value == 1) return true;     // 1 = 2^0

    int sum = 0;

    for(size_t i = 0; i < sizeof(value) * 8; i++)
        sum += (value >> i) & 0x01;

    return sum == 1;
}

//создание таблицы. replica_delimiter - дробность реплики. 1 - ПФЭ.
GradientFactorTable::GradientFactorTable(int factor_count, int row_count, int replica_delimiter) : FactorTable(factor_count, row_count)
{
    assert(factor_count > 0);
    assert(row_count > 1);
    assert(replica_delimiter > 0);
    assert(ispow2(replica_delimiter));
    assert(static_cast<int>(pow(2.0, factor_count)) / replica_delimiter > factor_count);    // число строк таблицы должно быть больше кол-ва факторов

    _replicaDelimiter = replica_delimiter;
    _factorCount = factor_count;
    /*
    if(replica_delimiter == 1)  // полный факторный эксперимент
        generateFullExperimentCoeffs();
    else // дробная реплика
    */
    {
        if (_replicaDelimiter != 1)
            _generalFactorCount = static_cast<int>(log2(1.0 / (_replicaDelimiter / pow(2., _factorCount))));
        else
            _generalFactorCount = factor_count; // справедливо для полнофакторного плана

        _replica_row_count = pow(2., _generalFactorCount);
        _generateRelations.resize(factor_count - _generalFactorCount);
        std::vector<std::vector<int> > tmp_relations;
        std::vector<int> relation;
        relation.push_back(1);
        relation.push_back(2);

        for(int factor_number = 0; factor_number < _generalFactorCount; factor_number++)
            for(int row_number = 0; row_number < _replica_row_count; row_number++)
            {
                int delimiter = static_cast<int>(pow(2., factor_number + 1));
                double value = (row_number % delimiter < delimiter / 2) ? -1.0 : 1.0;
                set_at(factor_number, row_number, value);
            }

        while(relation.size())
        {
            tmp_relations.push_back(relation);
            relation = nextVec(relation, _generalFactorCount, _generalFactorCount, 1);
        }

        for(int i = 0; i < factor_count - _generalFactorCount; i++)
            _generateRelations[i] = tmp_relations[tmp_relations.size() - i - 1];

        for(int factor_number = _generalFactorCount, i = 0; factor_number < factor_count; factor_number++, i++)
        {
            for(int row_number = 0; row_number < _replica_row_count; row_number++)
            {
                double r = 1;

                for(int j = 0; j < (int)_generateRelations[i].size(); j++)
                    r *= norm_at(_generateRelations[i][j] - 1, row_number);

                set_at(factor_number, row_number, r);
            }
        }
    }

}

// метод генерации таблицы факторов полного факторного эксперимента
void GradientFactorTable::generateFullExperimentCoeffs()
{
    for(int factor_number = 0; factor_number < count(); factor_number++)
        for(int row_number = 0; row_number < rowCount(); row_number++)
        {
            int delimiter = static_cast<int>(pow(2., factor_number + 1));
            double value = (row_number % delimiter < delimiter / 2) ? -1.0 : 1.0;
            set_at(factor_number, row_number, value);
        }
}

GradientFactorTable::~GradientFactorTable()
{
}

void GradientFactorTable::loadFrom(DataStream &srcStream)
{
    FactorTable::loadFrom(srcStream);
    srcStream >> _replicaDelimiter;
    srcStream >> _factorCount;
    srcStream >> _generalFactorCount;
    srcStream >> _replica_row_count;
    srcStream >> _interactionLevel;

    _generateRelations.clear();

    quint64 rows;
    srcStream >> rows;

    for(size_t i = 0; i < rows; i++)
    {
        quint64 cols;
        srcStream >> cols;
        std::vector<int> v;

        for(size_t j = 0; j < cols; j++)
        {
            int val;
            srcStream >> val;
            v.push_back(val);
        }

        _generateRelations.push_back(v);
    }
}

void GradientFactorTable::loadFromCSV(QTextStream &srcStream)
{
    FactorTable::loadFromCSV(srcStream);
    QString tmp;
    QStringList tmpList;
    double num;
    tmp = srcStream.readLine();
    if ("" == tmp.split(QRegExp(";"))[1]) {
        tmp = srcStream.readLine();
    }
    if ("" != tmp) {
        tmp = tmp.split(QRegExp(";"))[1];
        _factorCount = tmp.toInt();
        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        _replica_row_count = tmp.toInt();
        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        _replicaDelimiter = tmp.toInt();
        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        _generalFactorCount = tmp.toInt();

        _center = new std::vector<double>(_factorCount);
        _delta = new std::vector<double>(_factorCount);

        srcStream.readLine();
        for(size_t i = 0; i < _factorCount; i++)
        {
            tmpList = srcStream.readLine().split(QRegExp(";"));
            tmp = ExperimentTable::doubleWithDot(tmpList[1]);
            num = tmp.toDouble();
            _center->at(i) = num;
        }
        srcStream.readLine();
        for(size_t i = 0; i < _factorCount; i++)
        {
            tmpList = srcStream.readLine().split(QRegExp(";"));
            tmp = ExperimentTable::doubleWithDot(tmpList[1]);
            num = tmp.toDouble();
            _delta->at(i) = num;
        }

        _descriptions.clear();
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
                _descriptions << d.replace("\"","");
            }
        }
        QString intLevString = srcStream.readLine().split(QRegExp(";"))[1];
        _interactionLevel = intLevString.toInt();
        _generateRelations.clear();

        quint64 rows;
        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        rows = tmp.toLongLong();

        for(size_t i = 0; i < rows; i++)
        {
            quint64 cols;
            tmp = srcStream.readLine().split(QRegExp(";"))[1];
            cols = tmp.toLongLong();
            std::vector<int> v;

            for(size_t j = 0; j < cols; j++)
            {
                int val;
                tmp = srcStream.readLine().split(QRegExp(";"))[1];
                val = tmp.toInt();
                v.push_back(val);
            }
            _generateRelations.push_back(v);
        }

        std::vector<std::vector<int> > tmp_relations;
        std::vector<int> relation;
        relation.push_back(1);
        relation.push_back(2);
        _x = new vector<vector<double>* >(_replica_row_count);

        for(int row = 0; row < _replica_row_count; row++)
        {
            _x->at(row) = new vector<double>(_factorCount);

            for(int factor = 0; factor < _factorCount; factor++)
                _x->at(row)->at(factor) = std::numeric_limits<double>::quiet_NaN();
        }

        for(int factor = 0; factor < _factorCount; factor++) {
            for(int row = 0; row < _replica_row_count; row++) {
                int delimiter = static_cast<int>(pow(2., factor + 1));
                double value = (row % delimiter < delimiter / 2) ? -1.0 : 1.0;
                set_at(factor, row, value);
            }
        }

        while(relation.size())
        {
            tmp_relations.push_back(relation);
            relation = nextVec(relation, _generalFactorCount, _generalFactorCount, 1);
        }

        for(int i = 0; i < _factorCount - _generalFactorCount; i++)
            _generateRelations[i] = tmp_relations[tmp_relations.size() - i - 1];

        for(int factor_number = _generalFactorCount, i = 0; factor_number < _factorCount; factor_number++, i++)
        {
            for(int row_number = 0; row_number < _replica_row_count; row_number++)
            {
                double r = 1;

                for(int j = 0; j < (int)_generateRelations[i].size(); j++)
                    r *= norm_at(_generateRelations[i][j] - 1, row_number);

                set_at(factor_number, row_number, r);
            }
        }
    }
}

void GradientFactorTable::saveTo(DataStream &dstStream)
{
    FactorTable::saveTo(dstStream);

    dstStream << _replicaDelimiter;
    dstStream << _factorCount;
    dstStream << _generalFactorCount;
    dstStream << _replica_row_count;
    dstStream << _interactionLevel;

    dstStream << (quint64)_generateRelations.size();

    for(size_t i = 0; i < _generateRelations.size(); i++)
    {
        quint64 cols = _generateRelations.at(i).size();
        dstStream << cols;

        for(size_t j = 0; j < cols; j++)
        {
            dstStream << _generateRelations.at(i).at(j);
        }
    }
}

void GradientFactorTable::saveToCSV(QTextStream &dstStream)
{
    FactorTable::saveToCSV(dstStream);
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
    if (codec!= NULL)
    {
        dstStream.setCodec(codec);
    }
    else
    {
        assert(1);
    }

    dstStream << QString::fromUtf8("ТАБЛИЦА ФАКТОРОВ;\r\n");
    dstStream << QString::fromUtf8("Число факторов:") << ";" << (quint64)_x->at(0)->size() << "\r\n";
    dstStream << QString::fromUtf8("Число опытов ПФЭ:") << ';' << (quint64)_x->size() << ';' << "\r\n";
    dstStream << QString::fromUtf8("Дробность реплики:") << ";" << _replicaDelimiter << ";\r\n";
    dstStream << QString::fromUtf8("Число базовых факторов:") << ";" << _generalFactorCount << ";\r\n";
    dstStream << QString::fromUtf8("Центры плана:") << ';' << "\r\n";
    for(size_t i = 0; i < _center->size(); i++)
    {
        dstStream << ';' << ExperimentTable::doubleWithComma(_center->at(i)) << ';';
        dstStream << "\r\n";
    }
    dstStream << QString::fromUtf8("Диапазон варьирования:") << ';' << "\r\n";
    for(size_t i = 0; i < _center->size(); i++)
    {
        dstStream << ';' << ExperimentTable::doubleWithComma(_delta->at(i)) << ';';
        dstStream << "\r\n";
    }
    dstStream << QString::fromUtf8("Размер описаний:") << ';';
    quint64 sz = (quint64)_descriptions.size();
    dstStream << sz << ';' << "\r\n";

    dstStream << QString::fromUtf8("Описания:") << ';' << "\r\n";
    if(_descriptions.size() > 0)
    {
        for(size_t i = 0; i < sz; i++) {
            dstStream << ';' << _descriptions[i] << ";\r\n";
        }
    }
    dstStream << QString::fromUtf8("Уровень взаимодействия:") << ";" << (quint64)_interactionLevel << ";\r\n";
    dstStream << QString::fromUtf8("Количество генерирующих соотношений:") << ";" << (quint64)_generateRelations.size() << ";\r\n";
    for(size_t i = 0; i < _generateRelations.size(); i++)
    {
        quint64 cols = _generateRelations.at(i).size();
        dstStream << QString::fromUtf8("Размер генерирующих соотношений:") << ";" << cols << ";\r\n";

        for(size_t j = 0; j < cols; j++)
        {
            dstStream << QString::fromUtf8("Генерирующее соотношение ХЗ") << i << j << ":;" << _generateRelations.at(i).at(j) << ";\r\n";
        }
    }
}

// поддерживает ли реплики
bool GradientFactorTable::supportsReplica() const
{
    return !(_replicaDelimiter == 1);
}

// является ли фактор основным
bool GradientFactorTable::isGeneral(int factor_number) const
{
    assert(factor_number >= 0 && factor_number < count());
    // TODO: реализовать логику работы метода
    return factor_number < _generalFactorCount;
}

// кол-во основных факторов
int GradientFactorTable::generalFactorCount() const
{
    // TODO: реализовать логику работы метода

    return _generalFactorCount;
}

// установить генерирующее соотношение
void GradientFactorTable::setGenerateRelation(int number, const std::vector<int> &relation)
{
    assert(number >= 0 && number < count());
    assert(!isGeneral(number));
    // TODO: реализовать логику работы метода
    _generateRelations[number - generalFactorCount()] = relation;

    for(int row_number = 0; row_number < _replica_row_count; row_number++)
    {
        double r = 1;

        for(int j = 0; j < (int)_generateRelations[number - generalFactorCount()].size(); j++)
        {
            if(_generateRelations[number - generalFactorCount()][j] >= 0)
                r *= norm_at(abs(_generateRelations[number - generalFactorCount()][j]) - 1, row_number);
            else
                r *= -norm_at(abs(_generateRelations[number - generalFactorCount()][j]) - 1, row_number);
        }

        set_at(number, row_number, r);
    }
}

std::vector<int> GradientFactorTable::getGenerateRelation(int number) const
{
    assert(number >= 0 && number < count());

    //assert(!isGeneral(number)); //comented: not very convinient to use
    //suggest this:
    if(isGeneral(number))
    {
        std::vector<int> idx;
        idx.push_back(number + 1);
        return idx;
    }

    // TODO: реализовать логику работы метода
    int idx = number - generalFactorCount();
    assert(idx<_generateRelations.size()  && "_generateRelations in incorrect state!");
    return _generateRelations[idx];
}
