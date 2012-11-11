#ifndef REGRESSIONFUNCTIONS_H
#define REGRESSIONFUNCTIONS_H

#include <vector>

/* файл вспомогательных функций для расчета коэффициентов регрессии */

class IFactorTable;
class IResponseTable;
class IRegressionCoefficientTable;
class GradientRegressionCoefficientTable;
class CentralOrtogonalRegressionCoefficientTable;
class bCoeff;

/*-- функции для градиентного плана --*/

void calcGradientNormCoefs(const IFactorTable &xTable, const IResponseTable &yTable,
                           GradientRegressionCoefficientTable &bTable); // ф-ция расчета нормированных коэф-тов градиентного уравнения регресии

std::vector<bCoeff>* generateGradientCoeffs(const int factorCount, const int interactionLevel); // ф-ция генерации индексов коэфф-тов градиентного уравнения регрессии

// ф-ция расчета натуральных коэф-тов градиентного уравнения регрессии
void calcGradientNaturalCoefs(const IFactorTable &xTable, const IResponseTable &yTable,
                              GradientRegressionCoefficientTable &bTable);

// функция разбора нормированного коэффциента регрессии на составляющие натуральных коэффициентов
void parseGradientNormCoef(const IFactorTable &xTable, const IResponseTable &yTable,
                           GradientRegressionCoefficientTable &bTable, int coeffNumber);

// функция, считающая натуральный коэф-т градиентного уравнения регрессии
void calcGradientNaturalCoef(const IFactorTable &xTable, const IResponseTable &yTable,
                             GradientRegressionCoefficientTable &bTable, int index, int coeffNumber);

// функция поиска номера коэффицента уравениня регрессии по его индексу
int findCoefNumberByIndex(const IRegressionCoefficientTable &bTable, const std::vector<int> index);


/*-- функции для центрального композиционного плана --*/

void calcCOCPNormCoefs(const IFactorTable &xTable, const IResponseTable &yTable,
                       CentralOrtogonalRegressionCoefficientTable &bTable); // ф-ция расчета нормированных коэф-тов градиентного уравнения регресии

std::vector<bCoeff>* generateCOCPCoeffs(const int factorCount, const int interactionLevel); // ф-ция генерации индексов коэфф-тов градиентного уравнения регрессии

// ф-ция расчета натуральных коэф-тов градиентного уравнения регрессии
void calcCOCPNaturalCoefs(const IFactorTable &xTable, const IResponseTable &yTable,
                          CentralOrtogonalRegressionCoefficientTable &bTable);

// функция вывода натуральных коэф-тов из нормированного
void parseNormCoef(const IFactorTable &xTable, CentralOrtogonalRegressionCoefficientTable &bTable,
                   int index_in_table);

// функция, считающая натуральный коэф-т
void calcNatCoef(const IFactorTable &xTable, CentralOrtogonalRegressionCoefficientTable &bTable, int index, int index_in_table);

#endif // REGRESSIONFUNCTIONS_H
