#include "RegressionFunctions.h"
#include "GradientRegressionCoefficientTable.h"
#include "CentralOrtogonalRegressionCoefficientTable.h"
#include <assert.h>
#include <vector>
#include <limits>
#include <cmath>
#include "bCoeff.h"
#include "IResponseTable.h"
#include "IFactorTable.h"

using namespace std;


// ф-ция расчета нормированных коэф-тов уравнения регрессии
void calcGradientNormCoefs(const IFactorTable &xTable, const IResponseTable &yTable,
                           GradientRegressionCoefficientTable &bTable)
{
    double b;

    for(int i = 0; i < bTable.count(); i++)
    {
        b = 0.0;

        if(bTable.coeffs()[i].index()[0] == 0)
        {
            // b0
            for(int j = 0; j < xTable.rowCount(); j++)
                b += yTable.trusted_average_at(j);

            b /= (double)xTable.rowCount();
        }
        else
        {
            // other b
            for(int j = 0; j < xTable.rowCount(); j++)
            {
                double x = 1.0;

                for(int k = 0; k < (int)bTable.coeffs()[i].index().size(); k++)
                    x *= xTable.norm_at(bTable.coeffs()[i].index()[k] - 1, j);

                b += yTable.trusted_average_at(j) * x;
            }

            b /= (double)xTable.rowCount(); // instead of table->inputs
        }

        bTable.coeffs()[i].set_norm_value(b);
    }
}


// функция генерации индексов коэффициентов уравнения регрессии
vector<bCoeff>* generateGradientCoeffs(const int factorCount, const int interactionLevel)
{
    assert(factorCount >= interactionLevel);

    vector<bCoeff> *coeffs = new vector<bCoeff>();

    // добавляем b0
    vector<int> b0index;
    b0index.push_back(0);
    bCoeff *b0 = new bCoeff(
        b0index,
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::quiet_NaN(),
        false
    );

    coeffs->push_back(*b0);
    vector<int> currentIndex;
    currentIndex.push_back(0);

    while(true)
    {
        bool needToExtend = true;
        int lastIndex = currentIndex.size() - 1;

        for(int number = lastIndex; number >= 0; number--)
            if((number == lastIndex && currentIndex.at(number) < factorCount)
                    || (number < lastIndex && currentIndex.at(number) < currentIndex.at(number + 1) - 1))
            {
                currentIndex[number] += 1;

                for(size_t next = number + 1; next < currentIndex.size(); next++)
                    currentIndex[next] = currentIndex[number] + next - number;

                needToExtend = false;
                break;
            }

        if(needToExtend)
        {
            for(size_t number = 0; number < currentIndex.size(); number++)
                currentIndex[number] = number + 1;

            if(currentIndex.size() == 0)
                currentIndex.push_back(1);
            else
                currentIndex.push_back(currentIndex[currentIndex.size() - 1] + 1);

            if(currentIndex.size() > (size_t)interactionLevel) break;
        }

        bCoeff coeff(currentIndex,
                     std::numeric_limits<double>::quiet_NaN(),
                     std::numeric_limits<double>::quiet_NaN(),
                     false
                    );
        coeffs->push_back(coeff);
    }

    return coeffs;
}


void calcGradientNaturalCoefs(const IFactorTable &xTable, const IResponseTable &yTable,
                              GradientRegressionCoefficientTable &bTable)
{
    for(int i = 0; i < bTable.count(); i++)
        parseGradientNormCoef(xTable, yTable, bTable, i);
}


void parseGradientNormCoef(const IFactorTable &xTable, const IResponseTable &yTable,
                           GradientRegressionCoefficientTable &bTable, int coeffNumber)
{
    assert(coeffNumber >= 0 && coeffNumber < bTable.count());
    uint dim = bTable.coeffs()[coeffNumber].index().size();
    assert(dim < sizeof(int) * 8);

    for(int i = 0; i < pow(2.0, (int)dim); i++)
        calcGradientNaturalCoef(xTable, yTable, bTable, i, coeffNumber);
}


void calcGradientNaturalCoef(const IFactorTable &xTable, const IResponseTable &yTable,
                             GradientRegressionCoefficientTable &bTable, int index, int coeffNumber)
{
    vector<int> nat_index;
    double result = 1.0;
    int zero_count = 0; // coeff sign depends on zero count

    if(bTable.coeffs()[coeffNumber].index()[0] == 0
            && bTable.coeffs()[coeffNumber].index().size() == 1)
    {
        // b0
        if(index == 0)
        {
            // add normed b0 to natural b0
            bTable.coeffs()[coeffNumber].set_natural_value(
                bTable.coeffs()[coeffNumber].norm_value() +
                bTable.coeffs()[coeffNumber].natural_value());
        }

        return;
    }

    // not b0
    for(uint i = 0; i < bTable.coeffs()[coeffNumber].index().size(); i++)
        if(((index >> i) & 1) == 1)
            nat_index.push_back(bTable.coeffs()[coeffNumber].index()[i]);
        else
        {
            result *= xTable.xCenter(bTable.coeffs()[coeffNumber].index()[i] - 1);
            ++zero_count;
        }

    if(zero_count % 2 == 1) result *= -1.0;

    result *= bTable.coeffs()[coeffNumber].norm_value();  // * b[ij...] norm

    for(uint i = 0; i < bTable.coeffs()[coeffNumber].index().size(); i++)
        result /= xTable.xDelta(bTable.coeffs()[coeffNumber].index()[i] - 1);   // / delta(x[i])

    // find norm b by index
    if(nat_index.size() == 0) nat_index.push_back(0);   // if empty, that add to b0

    int bNumber = findCoefNumberByIndex(bTable, nat_index);
    assert(bNumber != -1);

    bTable.coeffs()[bNumber].set_natural_value(bTable.coeffs()[bNumber].natural_value() + result);   // add partial value to coefficient value in table
}


int findCoefNumberByIndex(const IRegressionCoefficientTable &bTable, const std::vector<int> index)
{
    assert(index.size() > 0);

    for(int coeffNumber = 0; coeffNumber < bTable.count(); coeffNumber++)
    {
        if(index.size() != bTable.coeffs()[coeffNumber].index().size()) continue;

        bool match = true;

        for(size_t indexDigit = 0; indexDigit < index.size(); indexDigit++)
            if(bTable.coeffs()[coeffNumber].index()[indexDigit] != index[indexDigit])
            {
                match = false;
                break;
            }

        if(match) return coeffNumber;
    }

    return -1;
}


/*==============================================================*/

// проверить коэффициенты на NaN
void calcCOCPNormCoefs(const IFactorTable &xTable, const IResponseTable &yTable,
                       CentralOrtogonalRegressionCoefficientTable &bTable)
{
    // расчет b[j], j != 0
    for(int b_number = 1; b_number <= xTable.count(); b_number++)
    {
        double numerator_sum = 0.0, denominator_sum = 0.0;

        for(int row_number = 0; row_number < xTable.rowCount(); row_number++)
        {
            numerator_sum += xTable.norm_at(b_number - 1, row_number) * yTable.trusted_average_at(row_number);
            denominator_sum += xTable.norm_at(b_number - 1, row_number) * xTable.norm_at(b_number - 1, row_number);
        }

        double b_value = numerator_sum / denominator_sum;
        vector<int> b_index;
        b_index.push_back(b_number);

        bCoeff *coeff = bTable.at(b_index);
        assert(coeff != NULL);
        coeff->set_norm_value(b_value);
    }

    // расчет b(ij), i != j
    for(int b_first_number  = 1; b_first_number <= xTable.count(); b_first_number++)
    {
        for(int b_second_number  = b_first_number + 1; b_second_number <= xTable.count(); b_second_number++)
        {
            double numerator_sum = 0.0, denominator_sum = 0.0;

            for(int row_number = 0; row_number < xTable.rowCount(); row_number++)
            {
                // TODO: проверить потом
                numerator_sum += xTable.norm_at(b_first_number - 1, row_number) * xTable.norm_at(b_second_number - 1, row_number) * yTable.trusted_average_at(row_number);
                denominator_sum += xTable.norm_at(b_first_number - 1, row_number) * xTable.norm_at(b_first_number - 1, row_number)
                                   * xTable.norm_at(b_second_number - 1, row_number) * xTable.norm_at(b_second_number - 1, row_number);
            }

            double b_value = numerator_sum / denominator_sum;
            vector<int> b_index;
            b_index.push_back(b_first_number);
            b_index.push_back(b_second_number);

            bCoeff *coeff = bTable.at(b_index);
            assert(coeff != NULL);
            coeff->set_norm_value(b_value);
        }
    }

    int N = xTable.rowCount();

    // расчет b[ii]
    for(int factor_number = 1; factor_number <= xTable.count(); factor_number++)
    {
        double numerator_sum = 0.0, denominator_sum = 0.0;

        for(int row_number = 0; row_number < N; row_number++)
        {
            double x_star = xTable.norm_at(factor_number - 1, row_number) * xTable.norm_at(factor_number - 1, row_number);
            double sum = 0.0;

            for(int row_number2 = 0; row_number2 < N; row_number2++)
                sum += xTable.norm_at(factor_number - 1, row_number2) * xTable.norm_at(factor_number - 1, row_number2);

            x_star -= sum / N;

            numerator_sum += x_star * yTable.trusted_average_at(row_number);
            denominator_sum += x_star * x_star;
        }

        // поделить и присвоить
        double b_value = numerator_sum / denominator_sum;
        vector<int> b_index;
        b_index.push_back(factor_number);
        b_index.push_back(factor_number);

        bCoeff *coeff = bTable.at(b_index);
        assert(coeff != NULL);
        coeff->set_norm_value(b_value);
    }

    // расчет b*[0]
    double b_star_0 = 0.0;

    for(int row_number = 0; row_number < xTable.rowCount(); row_number++)
        b_star_0 += yTable.trusted_average_at(row_number);

    b_star_0 /= xTable.rowCount();

    // расчет b[0]
    double b0 = b_star_0;

    for(int b_number = 1; b_number <= xTable.count(); b_number++)
    {
        double expr = 0.0;

        for(int row_number = 0; row_number < N; row_number++)
            expr += xTable.norm_at(b_number - 1, row_number) * xTable.norm_at(b_number - 1, row_number);

        vector<int> b_index;
        b_index.push_back(b_number);
        b_index.push_back(b_number);
        expr = expr * bTable.at(b_index)->norm_value() / N;

        b0 -= expr;
    }

    vector<int> b0_index;
    b0_index.push_back(0);

    bCoeff *coeff = bTable.at(b0_index);
    assert(coeff != NULL);
    coeff->set_norm_value(b0);
}


vector<bCoeff>* generateCOCPCoeffs(const int factorCount, const int interactionLevel)
{
    assert(factorCount > 1);

    vector<bCoeff> *coeffs = new vector<bCoeff>();

    // генерируем b0 .. bn
    for(int i = 0; i <= factorCount; i++)
    {
        vector<int> bIndex;
        bIndex.push_back(i);
        bCoeff b =  bCoeff(
                        bIndex,
                        std::numeric_limits<double>::quiet_NaN(),
                        std::numeric_limits<double>::quiet_NaN(),
                        false
                    );
        coeffs->push_back(b);
    }

    // генерируем b11 .. bnn
    for(int i = 1; i <= factorCount; i++)
        for(int j = i; j <= factorCount; j++)
        {
            vector<int> bIndex;
            bIndex.push_back(i);
            bIndex.push_back(j);
            bCoeff b =  bCoeff(
                            bIndex,
                            std::numeric_limits<double>::quiet_NaN(),
                            std::numeric_limits<double>::quiet_NaN(),
                            false
                        );
            coeffs->push_back(b);
        }

    return coeffs;
}


void calcCOCPNaturalCoefs(const IFactorTable &xTable, const IResponseTable &yTable,
                          CentralOrtogonalRegressionCoefficientTable &bTable)
{
    // обнуление натуральных значений коэф-тов
    for(int coef_number = 0; coef_number < bTable.count(); coef_number++)
        bTable.coeffs().at(coef_number).set_natural_value(0.0);

    for(int i = 0; i < bTable.count(); i++)
        parseNormCoef(xTable, bTable, i);
}

// функция вывода натуральных коэф-тов из нормированного
void parseNormCoef(const IFactorTable &xTable, CentralOrtogonalRegressionCoefficientTable &bTable,
                   int index_in_table)
{
    assert(index_in_table >= 0 && (size_t)index_in_table < bTable.coeffs().size());
    int dim = bTable.coeffs().at(index_in_table).index().size();

    assert(dim < (int)sizeof(int) * 8);

    for(int i = 0; i < static_cast<int>(pow(2.0, dim)); i++)
        calcNatCoef(xTable, bTable, i, index_in_table);
}

// функция, считающая натуральный коэф-т
void calcNatCoef(const IFactorTable &xTable, CentralOrtogonalRegressionCoefficientTable &bTable, int index, int index_in_table)
{
    vector<int> nat_index;
    double result = 1.0;
    int zero_count = 0; // coeff sign depends on zero count

    if(bTable.coeffs()[index_in_table].index()[0] == 0
            && bTable.coeffs()[index_in_table].index().size() == 1)
    {
        // b0
        if(index == 0)
        {
            bTable.coeffs()[index_in_table].set_natural_value(
                bTable.coeffs()[index_in_table].natural_value()
                + bTable.coeffs()[index_in_table].norm_value()
            );
        }

        return;
    }

    // not b0
    for(size_t i = 0; i < bTable.coeffs()[index_in_table].index().size(); i++)
        if(((index >> i) & 1) == 1)
            nat_index.push_back(bTable.coeffs()[index_in_table].index()[i]);
        else
        {
            result *= xTable.xCenter(bTable.coeffs()[index_in_table].index()[i] - 1);
            ++zero_count;
        }

    if(zero_count % 2 == 1) result *= -1.0;

    result *= bTable.coeffs()[index_in_table].norm_value();

    for(size_t i = 0; i < bTable.coeffs()[index_in_table].index().size(); i++)
        result /= xTable.xDelta(bTable.coeffs()[index_in_table].index()[i] - 1);

    // find norm b by index
    if(nat_index.size() == 0) nat_index.push_back(0);   // if empty, that add to b0

    bTable.at(nat_index)->set_natural_value(bTable.at(nat_index)->natural_value() + result);  // add partial value to coefficient value in table
}
