#ifndef COLA_H
#define COLA_H

#include <QDialog>

namespace Ui
{
class Cola;
}

class Cola : public QDialog
{
    Q_OBJECT

public:
    explicit Cola(QWidget *parent = 0);
    ~Cola();
    void getParams(double & a, int & b, int & c);

private slots:
    void on_doubleSpinBox_valueChanged(double arg1);

    void on_pushButton_clicked();

private:
    Ui::Cola *ui;
};

#endif // COLA_H
