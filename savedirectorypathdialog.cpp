#include "savedirectorypathdialog.h"
#include "ui_savedirectorypathdialog.h"

SaveDirectoryPathDialog::SaveDirectoryPathDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveDirectoryPathDialog)
{
   ui->setupUi(this);

    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
    PATH="";
    PATH=settings->value("settings/example_folder").toString();



    ui->CurSavePath->setText(PATH);
    ui->groupBox->hide();

}

SaveDirectoryPathDialog::~SaveDirectoryPathDialog()
{
    delete ui;    
}


void SaveDirectoryPathDialog::on_browse_dirs_clicked()
{


    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Путь к сохранённым данным"),
                                                    PATH,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if (dir!="")
    {
        ui->newSavePath->setText(dir);
        ui->groupBox->show();
    }

}

void SaveDirectoryPathDialog::on_OK_Button_clicked()
{
    QString textbefore=ui->CurSavePath->toPlainText();
    QString textafter = ui->newSavePath->toPlainText();

    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);

    if (textbefore==textafter)
    {
        this->close();
    }

    QString param ="";
    QDir dir = QDir::root();
    settings->setValue("settings/example_folder",textafter);
    QString ExStr=settings->value("settings/example_folder").toString();
    if (!dir.cd(ExStr)) dir.mkdir(ExStr);

    QString oldEXCEL =settings->value("settings/examle_CSV").toString();
    settings->setValue("settings/examle_CSV",ExStr+"/EXCEL");
    param=settings->value("settings/examle_CSV").toString();
     if (!dir.cd(param)) dir.mkdir(param);
     if (ui->cbxCopyData->checkState()==Qt::Checked)
     {
         CopyFilesInFrolder(oldEXCEL,param);
     }

    QString oldFormulas=settings->value("settings/examle_XML").toString();
    settings->setValue("settings/examle_XML",ExStr+"/FORMULAS");
    param=settings->value("settings/examle_XML").toString();
    if (!dir.cd(param)) dir.mkdir(param);
    if (ui->cbxCopyData->checkState()==Qt::Checked)
    {
        CopyFilesInFrolder(oldFormulas,param);
    }

    QString oldA_S=settings->value("settings/examle_XML").toString();
    settings->setValue("settings/examle_A_S",ExStr+"/PARAMS");
    param=settings->value("settings/examle_A_S").toString();
    if (!dir.cd(param)) dir.mkdir(param);
    if (ui->cbxCopyData->checkState()==Qt::Checked)
    {
        CopyFilesInFrolder(oldA_S,param);
    }

    settings->sync();



    this->close();
}

void SaveDirectoryPathDialog::on_Cancel_Button_clicked()
{
    this->close();
}


bool SaveDirectoryPathDialog::CopyFilesInFrolder(QString FromFolder,QString ToFolder)
{
    QDir dir = QDir::root();
    if (!dir.cd(ToFolder))
    {
        return false;
    }

    if (dir.cd(FromFolder))
    {
        foreach (QString FileName ,dir.entryList())
        {
            QFile *file = new QFile(FileName);
            file->copy(FromFolder+"\\"+FileName ,ToFolder+"\\"+FileName);
            file->close();
        }
        return true;
    }
    return false;
}



void SaveDirectoryPathDialog::on_buttonBox_accepted(){}

void SaveDirectoryPathDialog::on_pushButton_2_clicked(){}

void SaveDirectoryPathDialog::on_pushButton_clicked(){}

void SaveDirectoryPathDialog::on_toolButton_clicked(){}
