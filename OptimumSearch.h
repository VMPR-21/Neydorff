#ifndef OPTIMUMSEARCH_H
#define OPTIMUMSEARCH_H

#include <vector>

class IExperimentTable;

class OptimumSearch
{
public:
    static std::vector<double> FindByGradientMethod(const IExperimentTable &table,
            const std::vector<double> &x0, double eps, double eps1,
            /*double grad_length, double delta_length,*/
            double lambda, double beta, bool ismax);

private:
    OptimumSearch();
};

#endif // OPTIMUMSEARCH_H
