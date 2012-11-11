#include "OptimumSearch.h"
#include <assert.h>
#include <vector>
#include "IExperimentTable.h"
#include "IFactorTable.h"
#include <math.h>

using namespace std;

OptimumSearch::OptimumSearch()
{
}

// расчет приращения для аргументов
vector<double> calcDelta(const IFactorTable &xTable, double deltaMult)
{
    vector<double> delta;

    for(int i = 0; i < xTable.count(); i++)
        delta.push_back(xTable.xDelta(i) * 2 * deltaMult);

    return delta;
}

// функция расчета градиента
vector<double> calcGradient(const IExperimentTable &table, const vector<double> &x, const vector<double> &delta)
{
    vector<double> grad;

    for(size_t i = 0; i < x.size(); i++)
    {
        vector<double> x1(x);
        x1[i] += delta[i];
        grad.push_back((table.evalFuncValue(x1) - table.evalFuncValue(x)) / delta[i]);
    }

    return grad;
}

// расчет нормы (длины) вектора
double norm(const vector<double> &vect)
{
    double length;

    for(size_t i = 0; i < vect.size(); i++)
        length += vect[i] * vect[i];

    return sqrt(length);
}

// проверка, принадлежит ли точка области
bool InBounds(const vector<double> &point, const IFactorTable &xTable)
{
    for(size_t i = 0; i < point.size(); i++)
        if((point[i] < xTable.xCenter(i) - xTable.xDelta(i)) ||
                (point[i] > xTable.xCenter(i) + xTable.xDelta(i)))
            return false;

    return true;
}

// метод нормализации градиента
vector<double> normalizeGradient(const IExperimentTable &table, const vector<double> grad, double length)
{
    assert(length > 0.0);
    double max = grad[0];

    for(size_t i = 0; i < grad.size(); i++)
        if(fabs(grad[i]) > max) max = fabs(grad[i]);

    double delimiter = max / length;
    vector<double> result;

    for(size_t i = 0; i < grad.size(); i++)
        result.push_back(grad[i] / delimiter);

    return result;
}

// метод сдвига точки на градиент
vector<double> addGrad(const vector<double> &vect, const vector<double> &grad, double multiplier)
{
    vector<double> result;

    for(size_t i = 0; i < vect.size(); i++)
        result.push_back(vect[i] + grad[i] * multiplier);

    return result;
}

double maxByAbs(const vector<double> &vect)
{
    double max = 0.0;

    for(size_t i = 0; i < vect.size(); i++)
    {
        double current = fabs(vect[i]);

        if(current > max) max = current;
    }

    return max;
}

// поиск оптимума градиентным методом с дроблением шага
/*
  Описание параметров:
    table - таблица эксперимента (нужна для получения значений функции и границ области эксперимента) - не нужна в окне
    x0 - начальные координаты поиска оптимума
    eps - (эпсилон) погрешность нахождения оптимума
    eps1 - погрешность приращения функции
    grad_length - длина градиента (абсолютная)
    delta_length - длина приращения аргумента (относительная)
    lambda - начальный множитель шага
    beta - коэффициент дробления шага
    ismax - если true -  искать маскимум, false - минимум
*/
vector<double> OptimumSearch::FindByGradientMethod(const IExperimentTable &table,
        const vector<double> &x0, double eps, double eps1,
        /*double grad_length, double delta_length,*/
        double lambda, double beta, bool ismax)
{
    assert(x0.size() == (size_t)table.x().count());
    assert(eps > 0.0 && eps < 1.0);
    assert(eps1 > 0.0 && eps1 < 1.0);
    //assert(grad_length > 0.0);
    //assert(delta_length > 0.0 && delta_length < 0.5);
    assert(lambda > 0.0);
    assert(beta > 0.0 && beta < 1.0);

    double mult = ismax ? 1.0 : -1.0;
    //vector<double> delta = calcDelta(table.x(), delta_length);
    vector<double> delta = calcDelta(table.x(), 0.05);
    vector<double> x(x0);
    vector<double> grad = calcGradient(table, x, delta);

    while(norm(grad) >= eps && InBounds(x, table.x()))
    {
        double _lambda = lambda;
        //grad = normalizeGradient(table, grad, grad_length);
        grad = normalizeGradient(table, grad, maxByAbs(delta));

        while(table.evalFuncValue(addGrad(x, grad, _lambda)) - table.evalFuncValue(x)
                < eps1 * _lambda * pow(norm(grad), 2.0))
            _lambda *= beta;

        x = addGrad(x, delta, _lambda * mult);
    }

    return x;
}
