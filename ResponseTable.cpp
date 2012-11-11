#include "ResponseTable.h"
#include "assert.h"
#include "ICriteria.h"
#include "IExperimentTable.h"
#include "IFactorTable.h"
#include "IRegressionCoefficientTable.h"
#include "FactorTable.h"
#include "RegressionCoefficientTable.h"
#include "FisherCriteria.h"
#include "complex"
#include "BartleteCriteria.h"
#include "KohrenCriteria.h"
#include "ExperimentTable.h"

ResponseTable::ResponseTable(int row_count)
{
    assert(row_count > 0);
    m_values = vector<vector<YInfo> >();
    Resize(row_count);
}

ResponseTable::~ResponseTable()
{
}

void ResponseTable::Resize(int rows)
{
    assert(rows > 0);
    m_values.clear();
    m_values.resize(rows);
}

vector<YInfo> ResponseTable::at(int row) const
{
    assert(row >= 0);
    assert((size_t)row < m_values.size());
    return m_values[row];
}

vector<double> ResponseTable::trusted_values_at(int row) const
{
    assert(row >= 0);
    assert((size_t)row < m_values.size());
    vector<double> result;

    for(size_t i = 0; i < m_values[row].size(); i++)
    {
        if(m_values[row][i].IsTrusted == true)
            result.push_back(m_values[row][i].Value);
    }

    return result;
}

double ResponseTable::average_at(int row) const
{
    assert(row >= 0);
    assert((size_t)row < m_values.size());
    assert(m_values[row].size() > 0);
    double average = 0;

    for(size_t i = 0;  i < m_values[row].size(); i++)
    {
        average += m_values[row][i].Value;
    }

    return average / m_values[row].size();
}

double ResponseTable::trusted_average_at(int row) const
{
    assert(row >= 0);
    assert((size_t)row < m_values.size());
    assert(m_values[row].size() > 0);
    double average = 0;
    int count = 0;

    for(size_t i = 0;  i < m_values[row].size(); i++)
    {
        if(m_values[row][i].IsTrusted)
        {
            average += m_values[row][i].Value;
            count++;
        }
    }

    return average / count;
}

void ResponseTable::set_at(int row, vector<YInfo> values)
{
    assert(row >= 0);
    assert((size_t)row < m_values.size());
    m_values[row] = values;
}

void ResponseTable::add_at(int row, YInfo value)
{
    assert(row >= 0);
    assert((size_t)row < m_values.size());
    m_values[row].push_back(value);
}

void ResponseTable::add_at(int row, double value)
{
    assert(row >= 0);
    assert((size_t)row < m_values.size());
    YInfo info;
    info.Value = value;
    info.IsTrusted = true;
    m_values[row].push_back(info);
}

void ResponseTable::statProcessing(ICriteria &criteria, double trust_probability, CenterYDelegatePtr ysRequestDel, bool isFormulaModel)
{

    if ((m_values[0].size() > 1)||(isFormulaModel))   // если имеются || /* или новый эксперимент - функция */
    {
        for(size_t i = 0; i < m_values.size(); i++)
        {
            vector<double> values;

            for(size_t j = 0; j < m_values[i].size(); j++)
            {
                m_values[i][j].IsTrusted = false;
                values.push_back(m_values[i][j].Value);
            }

            vector<double> trusted = criteria.CheckValues(values, trust_probability);

            for(size_t j = 0; j < m_values[i].size(); j++)
            {
                //double val = m_values[i][j].Value;
                vector<double>::iterator index;
                index =  std::find(trusted.begin(), trusted.end(), m_values[i][j].Value);

                if(index != trusted.end())
                {
                    m_values[i][j].IsTrusted = true;
                    trusted.erase(index);
                }
            }
        }

        ongenericalDispersion(Bartlet, 0.95);
        dispersionVospr();
    }
    else // запрашиваем y в центре
    {
        yCenterRequest(ysRequestDel);
        vector<double> vals;

        for(size_t i = 0; i < m_yCenter.size(); i++)
            vals.push_back(m_yCenter[i].Value);

        vector<double> trusted = criteria.CheckValues(vals, trust_probability);

        for(size_t i = 0; i < m_yCenter.size(); i++)
        {
            double val = m_yCenter[i].Value;
            vector<double>::iterator index;
            index =  std::find(trusted.begin(), trusted.end(), val);

            if(index != trusted.end())
            {
                m_yCenter[i].IsTrusted = true;
                trusted.erase(index);
            }
        }
    }
}


int ResponseTable::count() const
{
    return m_values.size();
}



double ResponseTable::dispersion(int row) const
{
    assert(row >= 0);
    assert((size_t)row < m_values.size());
    vector<double> trusted = trusted_values_at(row);

    if(trusted.size() < 2)
        return 0;

    double average = trusted_average_at(row);
    double disp = 0;

    for(size_t i = 0; i < trusted.size(); i++)
        disp += (average - trusted[i]) * (average - trusted[i]);

    return disp / (trusted.size() - 1);
}


double ResponseTable::dispersionVospr() const
{
    assert(m_values.size() > 0);

    if(m_values[0].size() < 2)
    {
        return disp_at_center();
    }

    assert(m_values[0].size() > 1);
    double result = 0;
    int count = 0;

    for(size_t i = 0; i < m_values.size(); i++)
    {
        int freedoms  = trusted_values_at(i).size() - 1;
        double disp = dispersion(i);
        result += disp * freedoms;
        count += freedoms;
    }

    result = result / (double)count;
    return result;
}



int ResponseTable::ongenericalDispersion(StaticCriteria criteria, double prob)
{
    if(m_values.size() < 1 || m_values[0].size() < 2)
        return 1;

    vector<double> dispersions;

    for(size_t i = 0; i < m_values.size(); i++)
        dispersions.push_back(dispersion(i));

    if(criteria == Fisher)
    {
        double minD = dispersions[0], maxD = dispersions[0];
        int minF = trusted_values_at(0).size() - 1, maxF = trusted_values_at(0).size() - 1;

        for(size_t i = 1; i < dispersions.size(); i++)
        {
            double d = dispersions[i];
            int f = trusted_values_at(i).size() - 1;

            if(d < minD)
            {
                minD = d;
                minF = f;
            }

            if(d > maxD)
            {
                maxD = d;
                maxF = f;
            }
        }

        double Ft = FisherCriteria::GetFisherVal(minF, maxF);
        double F = 0;

        if(minD == 0)
            return false;
        else
            F =  maxD / minD;

        return Ft > F ? 1 : 0;
    }
    else if(criteria == Kohren)
    {
        size_t trusted = trusted_values_at(0).size() - 1;

        for(size_t i = 1; i < dispersions.size(); i++)
            if(trusted != trusted_values_at(i).size() - 1)
            {
                error = "Проверяемые дисперсии имеют разные степени свободы";
                return -1;
            }


        double maxDisp = 0, dispSum = 0;

        for(size_t i = 0; i < dispersions.size(); i++)
        {
            dispSum += dispersions[i];

            if(maxDisp < dispersions[i])
                maxDisp = dispersions[i];
        }

        double Cr = maxDisp / dispSum;
        double tableVal = KohrenCriteria::GetCriteriaVal(dispersions.size(), trusted_values_at(0).size() - 1);
        return Cr < tableVal ? 1 : 0;
    }
    else if(criteria == Bartlet)
    {
        for(size_t i = 0; i < dispersions.size(); i++)
            if(trusted_values_at(i).size() - 1 < 3)
            {
                error = "Степени свободы всех дисперсий должны быть больше 3х";
                return -1;
            }

        double svDisp = 0;
        int f_sum = 0;

        for(size_t i = 0; i < dispersions.size(); i++)
        {
            svDisp += dispersions[i] * (trusted_values_at(i).size() - 1);
            f_sum += trusted_values_at(i).size() - 1;
        }

        svDisp /= f_sum;

        if(svDisp == 0)
            return 1;

        double temp = 0;

        for(size_t i = 0; i < dispersions.size(); i++)
            temp += (trusted_values_at(i).size() - 1) * log10(dispersions[i]);

        temp = f_sum * log10(svDisp) - temp;
        temp *= 2.303;                         // chislitel

        double tmp = 0;

        for(size_t i = 0; i < dispersions.size(); i++)
            tmp += 1 / (double)(trusted_values_at(i).size() - 1);

        tmp -= 1 / (double)f_sum;
        tmp *= 1 / (double)(3 * (dispersions.size() - 1));
        tmp += 1;

        double B = temp / tmp;

        double val = BartleteCriteria::GetCriteriaVal((int)(f_sum / dispersions.size()), prob);

        return val > B ? 1 : 0;
    }



}


void ResponseTable::saveTo(DataStream &dstStream)
{
    dstStream << (quint64)m_values.size();

    for(size_t i = 0; i < m_values.size(); i++)
    {
        dstStream << (quint64)m_values[i].size();

        for(size_t j = 0; j < m_values[i].size(); j++)
        {
            dstStream << m_values[i][j].Value;
            dstStream << m_values[i][j].IsTrusted;
        }
    }
}

void ResponseTable::saveToCSV(QTextStream &dstStream)
{
    dstStream << QString::fromUtf8("ТАБЛИЦА ОТКЛИКОВ\r\n");
    dstStream << QString::fromUtf8("Количество параллельных опытов:") << ";" << (quint64)m_values[0].size() << "\r\n";
    for(size_t i = 0; i < m_values.size(); i++)
    {
        for(size_t j = 0; j < m_values[i].size(); j++)
        {
            dstStream << ";" << ExperimentTable::doubleWithComma(m_values[i][j].Value);
        }
        dstStream << "\r\n";
    }
}

void ResponseTable::loadFrom(DataStream &srcStream)
{
    quint64 rows;
    srcStream >> rows;
    Resize(rows);

    for(size_t i = 0; i < rows; i++)
    {
        quint64 size;
        srcStream >> size;
        _paral = size;

        for(size_t j = 0; j < size; j++)
        {
            double value;
            bool IsTrusted;
            srcStream >> value;
            srcStream >> IsTrusted;
            YInfo yi;
            yi.Value = value;
            yi.IsTrusted = IsTrusted;
            m_values[i].push_back(yi);
        }
    }
}

void ResponseTable::loadFromCSV(QTextStream &srcStream)
{
    quint64 rows, size;
    QString tmp;
    QStringList tmpList;
    srcStream.readLine();
    tmp = srcStream.readLine();
    if ("" != tmp) {
        tmp = tmp.split(QRegExp(";"))[1];
        rows = tmp.toULongLong();
        Resize(rows);
        tmp = srcStream.readLine().split(QRegExp(";"))[1];
        size = tmp.toULongLong();
        _paral = size;
        for(size_t i = 0; i < rows; i++)
        {
            tmpList = srcStream.readLine().split(QRegExp(";"));
            if (1 < tmpList.length()) {
                for(size_t j = 0; j < size; j++)
                {
                    double value;
                    bool IsTrusted;
                    tmp = ExperimentTable::doubleWithDot(tmpList[1 + j]);
                    value = tmp.toDouble();
                    YInfo yi;
                    yi.Value = value;
                    yi.IsTrusted = '1';
                    m_values[i].push_back(yi);
                }
            }
        }
    }
}

bool ResponseTable::checkAdec(const IExperimentTable &table)
{
    if(m_values[0].size() < 2)
        return true;

    IFactorTable &factors = table.x();

    vector<double> y_teor;

    for(size_t i = 0; i < (size_t)factors.rowCount(); i++)
    {
        vector<double> rowX;

        for(size_t j = 0; j < (size_t)factors.count(); j++)
            rowX.push_back(factors.natural_at(j, i));

        y_teor.push_back(table.evalFuncValue(rowX));
    }

    double aDisp = 0;

    for(size_t i = 0; i < y_teor.size(); i++)
        aDisp += (y_teor[i] - trusted_average_at(i)) * (y_teor[i] - trusted_average_at(i));

    aDisp /= (double)count();

    double vDisp = dispersionVospr();

    if(aDisp < vDisp)
        return true;

    double F = aDisp / vDisp;
    double Ft = FisherCriteria::GetFisherVal(count() - 1, count() - 1);

    return F < Ft;


}

void ResponseTable::yCenterRequest(CenterYDelegatePtr ysRequestDel)
{
    vector<double> vec;

    while(true)
    {
        vec = ysRequestDel->fire(vec);

        if(vec.size() > 1)
            break;
    }

    for(size_t i = 0; i < vec.size(); i++)
    {
        YInfo yi;
        yi.Value = vec[i];
        yi.IsTrusted = true;
        m_yCenter.push_back(yi);
    }
}

const char *ResponseTable::getLastError()
{
    return error;
}

vector<double> ResponseTable::trusted_at_center() const
{
    vector<double> vec;

    for(size_t i = 0; i < m_yCenter.size(); i++)
    {
        if(m_yCenter[i].IsTrusted)
            vec.push_back(m_yCenter[i].Value);
    }

    return vec;
}

double ResponseTable::trusted_average_at_center() const
{
    vector<double> vec = trusted_at_center();
    double sum = 0;

    for(size_t i = 0; i < vec.size(); i++)
        sum += vec[i];

    return sum / (double) vec.size();
}

double ResponseTable::disp_at_center() const
{
    vector<double> vec = trusted_at_center();
    double average = trusted_average_at_center();
    double sum = 0;

    for(size_t i = 0; i < vec.size(); i++)
        sum += (average - vec[i]) * (average - vec[i]);

    return sum / (double)(vec.size() - 1);
}

int ResponseTable::getParal()
{
    return this->_paral;
}
