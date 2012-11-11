#ifndef NEWEXPERIMENTDIALOG_H
#define NEWEXPERIMENTDIALOG_H

#include <QDialog>

namespace Ui {
class newExperimentDialog;
}

class newExperimentDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit newExperimentDialog(QWidget *parent = 0);
    ~newExperimentDialog();
    
private:
    Ui::newExperimentDialog *ui;
};

#endif // NEWEXPERIMENTDIALOG_H
