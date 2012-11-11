#ifndef BCOEFF_H
#define BCOEFF_H

#include <vector>

// класс для хранения коэффициента регрессии
class bCoeff
{
public:
    //Индекс коэф-та, например: B123[3] = {1,2,3}
    std::vector<int> index() const;

    double norm_value() const;//нормированое значение
    double natural_value() const;//натуральное значение
    bool is_significant() const;//является ли значимым коэффициентом

    void set_norm_value(double value);
    void set_natural_value(double value);
    void set_is_significant(bool value);

    bCoeff(const std::vector<int> &_index, double _norm_value, double _natural_value, bool _is_significant);

private:
    std::vector<int> _index;
    double _normed_value;
    double _natural_value;
    bool _is_significant;
};

#endif // BCOEFF_H
