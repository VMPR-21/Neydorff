#ifndef IPROBABILITYSELECTIONVIEW_H
#define IPROBABILITYSELECTIONVIEW_H

#include <vector>

class IProbabilitySelectionView
{
public:

    //возвращает индекс выбранного пользователем числа из probs
    virtual int askForProbability(const std::vector<double> &probs) = 0;

    virtual ~IProbabilitySelectionView() {}
};

#endif // IPROBABILITYSELECTIONVIEW_H
