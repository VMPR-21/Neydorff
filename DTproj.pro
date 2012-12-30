#-------------------------------------------------
#
# Project created by QtCreator 2012-04-10T17:59:08
#
#-------------------------------------------------

QT       += core gui script xml

TARGET = DTproj
#CONFIG   += console
CONFIG   -= app_bundle

#CONFIG -= exceptions rtti
#CONFIG += static

TEMPLATE = app

SOURCES += main.cpp \
    FactorTable.cpp \
    IResponseTable.cpp \
    IRegressionCoefficientTable.cpp \
    IFactorTable.cpp \
    IExperimentTable.cpp \
    ICriteria.cpp \
    GradientFactorTable.cpp \
    ExperimentTable.cpp \
    bCoeff.cpp \
    RegressionFunctions.cpp \
    RegressionCoefficientTable.cpp \
    GradientRegressionCoefficientTable.cpp \
    ResponseTable.cpp \
    NumberFormatDialog.cpp \
    CenterExperimentsDialog.cpp \
    NumberFormatter.cpp \
    UiController.cpp \
    mainwindow.cpp \
    ClipCopyPaste.cpp \
    Shuffler.cpp \
    FactorSettingsDialog.cpp \
    CentralOrtogonalRegressionCoefficientTable.cpp \
    ResponcesSource.cpp \
    StudentCriteria.cpp \
    StudentChecker.cpp \
    CentralOrtogonalFactorTable.cpp \
    FisherCriteria.cpp \
    BartleteCriteria.cpp \
    KohrenCriteria.cpp \
    ProbabilitySelectionDialog.cpp \
    Experementator.cpp \
    OptimumSearch.cpp \
    OptimumSearchSettingsDialog.cpp \
    ImaginarySteps.cpp \
    CalcFuncValueDialog.cpp \
    ExperimentDialog.cpp \
    cola.cpp \
    ResponcesSourseFunction.cpp \    
    formula_setting_help_dialog.cpp \
    findExtrDialog.cpp \
    NewExperimentSettingsDialog.cpp \
    startwindow.cpp \
    savedirectorypathdialog.cpp

HEADERS += \
    TableType.h \
    FactorTable.h \
    IResponseTable.h \
    IRegressionCoefficientTable.h \
    IFactorTable.h \
    IExperimentTable.h \
    ICriteria.h \
    GradientFactorTable.h \
    ExperimentTable.h \
    DataStream.h \
    bCoeff.h \
    RegressionFunctions.h \
    RegressionCoefficientTable.h \
    GradientRegressionCoefficientTable.h \
    ResponseTable.h \
    NumberFormatter.h \
    IModelSettingsView.h \
    NumberFormatDialog.h \
    INumberFormatView.h \
    INumberFormatter.h \
    ICenterExperimentsView.h \
    CenterExperimentsDialog.h \
    UiController.h \
    IView.h \
    IUiController.h \
    ClipCopyPaste.h \
    mainwindow.h \
    Shuffler.h \
    IFactorSettingsView.h \
    FactorSettingsDialog.h \
    CentralOrtogonalRegressionCoefficientTable.h \
    ResponcesSource.h \
    IResponcesSource.h \
    Events.h \
    StudentCriteria.h \
    StudentChecker.h \
    CentralOrtogonalFactorTable.h \
    FisherCriteria.h \
    BartleteCriteria.h \
    KohrenCriteria.h \
    ProbabilitySelectionDialog.h \
    IProbabilitySelectionView.h \
    IExperementator.h \
    Experementator.h \
    OptimumSearch.h \
    OptimumSearchSettingsDialog.h \
    ImaginarySteps.h \
    CalcFuncValueDialog.h \
    IExperimentDialog.h \
    ExperimentDialog.h \
    cola.h \
    ResponcesSourseFunction.h \
    formula_setting_help_dialog.h \
    findExtrDialog.h \
    NewExperimentSettingsDialog.h \
    startwindow.h \
    savedirectorypathdialog.h

FORMS += \
    FactorSettingsDialog.ui \
    CenterExperimentsDialog.ui \
    NumberFormatDialog.ui \
    mainwindow.ui \
    ProbabilitySelectionDialog.ui \
    OptimumSearchSettingsDialog.ui \
    ImaginarySteps.ui \
    CalcFuncValueDialog.ui \
    ExperimentDialog.ui \
    cola.ui \
    formula_setting_help_dialog.ui \
    findExtrDialog.ui \
    NewExperimentSettingsDialog.ui \
    startwindow.ui \
    savedirectorypathdialog.ui

#QMAKE_CXXFLAGS_RELEASE -= -O2
*-g++: QMAKE_CXXFLAGS += #-Wextra -pedantic -std=c++0x
#QMAKE_CXXFLAGS += -O3 -march=native -mfpmath=sse -ffast-math

RESOURCES += \
    formula_settings_dialog.qrc
