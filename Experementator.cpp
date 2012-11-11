#include "Experementator.h"
#include "IExperimentTable.h"
#include "IFactorTable.h"
#include "IRegressionCoefficientTable.h"
#include "bCoeff.h"
#include <math.h>
#include "Events.h"

Experementator::Experementator(double _h, unsigned int _ch, double _dev)
{
    Experementator::h = _h;
    Experementator::ch = _ch;
    Experementator::dev = _dev;
}

std::vector<ExperimentPoint> Experementator::calcStepYt(bool isMax, const IExperimentTable &table, ExperimentStepsDelegatePtr experimentDelegate, bool* isExtremFound)
{
    /*
    if(!table.b().count())
        return 0;
    */

    std::vector<bCoeff> bCoefs = table.b().coeffs();

    std::vector<double> grad;

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

    double xP = 0;  //шаг, начиная с какого перевалило за 1
    bool xP_find = false;
    double yt;
    ExperimentPoint P;
    std::vector<ExperimentPoint> v;

    std::vector<double> vecHbuf;    //вектор шагов

    for(unsigned int i = 0; i < grad.size(); i++)
    {
        vecHbuf.push_back(0);
    }

    //расчет xs и функции yt
    for (unsigned int i = 0; i < ch; i++)
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

        for(size_t i = 0; i < P.xs.size(); i++)
            P.xs[i] = table.x().xCenter(i) + table.x().xDelta(i) * P.xs[i];

        P.yt = table.evalFuncValue(P.xs);
        v.push_back(P);
    }

    /*
    (v[0]).yt=1363;
    (v[1]).yt=1401;
    (v[2]).yt=1439;
    (v[3]).yt=1477;
    (v[4]).yt=1515;
    (v[5]).yt=1510;
    (v[6]).yt=1500;

    (v[0]).yp=1364;
    (v[1]).yp=1402;
    (v[2]).yp=1440;
    (v[3]).yp=1478;
    (v[4]).yp=2516;
    (v[5]).yp=1511;
    (v[6]).yp=1501;
    */

    v = experimentDelegate->fire(v);

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

        //после расчета практических знчений определим есть превышение допустимого отклонения yt от yp
        for(size_t i = xP; i < v.size(); i++)
        {
            if(dev < (fabs(((v[i]).yt / (v[i]).yp) - 1) * 100)) //((yt/yp)-1)*100% - перевод в проценты
            {
                //есть превышение допустимого отклонения yt от yp

                //проводим эксперимент для получения уточненного направления градиента в этой точке
                v.erase (v.begin() + i + 1, v.end()); //если есть превышение, то удалим все после мах

                *isExtremFound = false;
                return v;
            }
        }

        //если нет превышения допустимого отклонения, то return
        v.erase (v.begin() + tp + 1, v.end());
        *isExtremFound = true;

        return v;// успех
    }
    else
    {
        //если нет точки перегиба, то проводим эксперимент в последней точке

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
                }
            }
        }

        if(prev == true) v.erase (v.begin() + xI + 1, v.end()); //если есть превышение, то удалим все после мах

        *isExtremFound = false;

        return v;
    }

    //return v.size()?v[v.size()-1].yt:0;
}
