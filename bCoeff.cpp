#include "bCoeff.h"
#include <vector>
#include <assert.h>
#include <cstdlib>

using namespace std;

bCoeff::bCoeff(const std::vector<int> &_index, double _norm_value, double _natural_value, bool _is_significant)
{
    this->_index = _index;
    this->_is_significant = _is_significant;
    this->_natural_value = _natural_value;
    this->_normed_value = _norm_value;
}

vector<int> bCoeff::index() const
{
    //assert(_index != NULL);
    return _index;
}

double bCoeff::norm_value() const
{
    return _normed_value;
}

double bCoeff::natural_value() const
{
    return _natural_value;
}

bool bCoeff::is_significant() const
{
    return _is_significant;
}

void bCoeff::set_norm_value(double value)
{
    _normed_value = value;
}

void bCoeff::set_natural_value(double value)
{
    _natural_value = value;
}

void bCoeff::set_is_significant(bool value)
{
    _is_significant = value;
}
