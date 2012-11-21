#ifndef SAVEDIRECTORYPATHDIALOG_H
#define SAVEDIRECTORYPATHDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QSettings>

namespace Ui {
class SaveDirectoryPathDialog;
}

class SaveDirectoryPathDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SaveDirectoryPathDialog(QWidget *parent = 0);
    ~SaveDirectoryPathDialog();
    
private slots:
    void on_buttonBox_accepted();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_toolButton_clicked();

    void on_browse_dirs_clicked();

    void on_OK_Button_clicked();

    void on_Cancel_Button_clicked();

private:
    Ui::SaveDirectoryPathDialog *ui;
    QString PATH;


   bool CopyFilesInFrolder(QString FromFolder,QString ToFolder);
};

#endif // SAVEDIRECTORYPATHDIALOG_H
