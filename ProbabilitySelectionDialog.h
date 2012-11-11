#ifndef PROBABILITYSELECTIONDIALOG_H
#define PROBABILITYSELECTIONDIALOG_H

#include <QDialog>
#include "IProbabilitySelectionView.h"

namespace Ui
{
class ProbabilitySelectionDialog;
}

class ProbabilitySelectionDialog : public QDialog, public IProbabilitySelectionView
{
    Q_OBJECT

public:
    explicit ProbabilitySelectionDialog(QWidget *parent = 0);
    ~ProbabilitySelectionDialog();

    int askForProbability(const std::vector<double> &probs);

private slots:
    void on_okPushButton_clicked();

private:
    Ui::ProbabilitySelectionDialog *ui;
};

#endif // PROBABILITYSELECTIONDIALOG_H
