#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QDialog>

namespace Ui {
class startWindow;
}

class startWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit startWindow(QWidget *parent = 0);
    ~startWindow();
    int showStartMenu();

private slots:

    void on_clbNewExperiment_clicked();

    void on_clbOpenExperiment_clicked();

    void on_clbImportFromCSV_clicked();

    void on_clbNewExperimentModeling_clicked();

    void on_clbNewExperimentExtrFind_clicked();

private:
    Ui::startWindow *ui;
    int ActionNumber;
};

#endif // STARTWINDOW_H
