#ifndef RESPONSETABLE_H
#define RESPONSETABLE_H
#include "IResponseTable.h"
#include "vector"

using namespace std;
struct YInfo;

class ResponseTable : public IResponseTable
{
private:
    vector<vector<YInfo> > m_values;
    vector<YInfo> m_yCenter;
    const char* error;
public:
    ResponseTable(int row_count);

    ~ResponseTable();

    void Resize(int rows); // установливаем кол-во строк в таблице

    vector<YInfo> at(int row) const;
    vector<double> trusted_values_at(int row) const;
    double average_at(int row) const;
    double trusted_average_at(int row) const;

    void set_at(int row, vector<YInfo> values);
    void add_at(int row, YInfo value);
    void add_at(int row, double value);

    int count() const;
    double dispersion(int row) const;

    // проверка однородности дисперсии
    int ongenericalDispersion(StaticCriteria criteria, double prob);

    double dispersionVospr() const;

    bool checkAdec(const IExperimentTable &table);

    void statProcessing(ICriteria &criteria, double trust_probability, CenterYDelegatePtr ysRequestDel,bool isFormulaModel);

    //загрузка и сохранение в поток данных таблицы
    //для помещения в поток и извлечения из потока использовать операторы >> и <<
    void loadFrom(DataStream &srcStream);
    void saveTo(DataStream &dstStream);

    //загрузка и сохранение в CSV
    void loadFromCSV(QTextStream &srcStream);
    void saveToCSV(QTextStream &dstStream);

    int getParal();

    vector<double> trusted_at_center() const;
    double trusted_average_at_center() const;
    double disp_at_center() const;

    const char* getLastError();
private:
    int _paral;
    void yCenterRequest(CenterYDelegatePtr ysRequestDel);
};

#endif // RESPONSETABLE_H
