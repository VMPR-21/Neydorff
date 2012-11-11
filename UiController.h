#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include "IUiController.h"
#include "IView.h"
#include "UiController.h"
#include <vector>
#include "IExperementator.h"

class INumberFormatView;
class INumberFormatter;
class IResponcesSource;
class IExperimentTable;

//Класс-контроллер для обеспечения взаимодействия между GUI и внутренними объектами.
class UiController : public IUiController
{
    bool _modelHasData;
    bool _isFormulaModel;
    IResponcesSource *_dataSrc;
    INumberFormatView *_formatterView;
    INumberFormatter *_formatter;
    IView *_view;
    IExperimentTable *_experimentTable;
    int _interactionLevel;
    int _paral;

    UiController(const UiController &c);
    UiController& operator=(const UiController &c);

    std::vector<double> centerYrequester(const std::vector<double> &centerVals);
    std::vector<ExperimentPoint> experimentalResponcesRequester(const std::vector<ExperimentPoint> &centerVals);
    std::vector<double> getProbsList(StaticCriteria criteria);
    void initEmptyModel();
    void initEmptyModelGrad();
    void setDataSource(int baseFactors, int factorsCount, const std::vector<double> &centers,
                       const std::vector<double> &deltas, QStringList descriptions,
                       QString displeyFunction = "",
                       QString cornerMeasure = "radians");
public:
    UiController(IView *view);
    ~UiController();

    IExperimentTable * getExperimentTable();
    void startProcess(bool max, double _h, int _ch, int _dev);
    void CCP();

    //Реализация IUiController
    bool newModel(bool createEmpty = false, bool isFormulaModel = false);
    //void newModel(bool createEmpty = false, bool isFormulaModel = false);

    void newModellGrad(bool _isFormulaModel2);

    bool loadModel(const QString &fileName);

    bool saveModel(const QString &fileName);

    void calcY(double threshold);

    void calcB(double threshold);

    INumberFormatter* getNumberFormatter();

    void changeNumberFormater();

    void changeFactorParams(int factorIndex);

    void calcExtremum(bool isMax);

    void calcAdeq();

    int checkHomogeneity(StaticCriteria criteria, const char **strErr);

    std::vector<double> gradientSearch();

    void calcFuncValue();

    //void changeModelParams();
    bool changeModelParams();

    void changeModelParamsGrad();

    void dataChanged();

    void needRefresh(bool inputsOnly = true);

    void changeBcoefSignificance(int row);

    void findExtr();
};

#endif // UICONTROLLER_H
