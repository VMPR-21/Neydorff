#include "StudentChecker.h"
#include "cmath"
#include "StudentCriteria.h"


StudentChecker::StudentChecker()
{
}

const char* StudentChecker::name() const
{
    return "student";
}

vector<double> StudentChecker::CheckValues(vector<double> values, double prob) const
{
    double average, checkingValue, sko;
    int index;

    while(values.size() > 2)
    {
        average = Average(values);
        index = 0;
        index = SuspiciousValue(values, average);
        checkingValue = values[index];

        if(checkingValue == average)
            break;

        values.erase(values.begin() + index);

        sko = 0;
        average = Average(values);

        for(size_t i = 0; i < values.size(); i++)
            sko += (values[i] - average) * (values[i] - average);

        sko = sko / (values.size() - 1);
        sko = sqrt(sko);


        if(sko == 0)
        {
            continue;
        }

        if(abs(checkingValue - average) / sko < StudentCriteria::GetStudentVal(values.size() - 1, prob))
        {
            values.push_back(checkingValue);
            break;
        }
    }

    return values;
}

double StudentChecker::Average(vector<double> &mas) const
{
    double average = 0;

    for(size_t i = 0; i < mas.size(); i++)
        average += mas[i];

    average /= mas.size();
    return average;
}

int StudentChecker::SuspiciousValue(vector<double> &mas, double average) const
{
    double max = 0;
    int index = 0;

    for(size_t i = 0; i < mas.size(); i++)
    {
        if(abs(average - mas[i]) > max)
        {
            max = abs(average - mas[i]);
            index = i;
        }
    }

    return index;
}

