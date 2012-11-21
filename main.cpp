#include <QApplication>
#include <QThread>
#include <QTextCodec>
#include <QSettings>
#include <QDir>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mainwindow.h>
#include "FactorTable.h"
#include "ResponseTable.h"

using namespace std;

class QMyThread: QThread
{
public:
    static void msleep(int ms)
    {
        QThread::msleep(ms);
    }
};

void debugHandler(QtMsgType type, const char *msg)
{
    switch (type)
    {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s\n", msg);
        break;

    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", msg);
        break;

    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        break;

    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg);
        //__asm("int3");
        abort();
        break;
    }
}

int main(int argc, char *argv[])
{
    qInstallMsgHandler(debugHandler);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QSettings *settings = new QSettings("settings.conf",QSettings::IniFormat);
    QString ExStr=settings->value("settings/example_folder").toString();
    if (ExStr=="")
    {
        QString param ="";
        QDir dir = QDir::root();
        QString apppath=QCoreApplication::applicationDirPath();
        if (apppath=="")
        {
           apppath= dir.currentPath();
        }
        settings->setValue("settings/example_folder",apppath+"/SAVED_DATA");
        ExStr=settings->value("settings/example_folder").toString();
        if (!dir.cd(ExStr)) dir.mkdir(ExStr);

        settings->setValue("settings/examle_CSV",ExStr+"/EXCEL");
        param=settings->value("settings/examle_CSV").toString();
         if (!dir.cd(param)) dir.mkdir(param);

        settings->setValue("settings/examle_XML",ExStr+"/FORMULAS");
        param=settings->value("settings/examle_XML").toString();
        if (!dir.cd(param)) dir.mkdir(param);

        settings->setValue("settings/examle_A_S",ExStr+"/EXPERIMENT");
        param=settings->value("settings/examle_A_S").toString();
        if (!dir.cd(param)) dir.mkdir(param);

        settings->sync();
    }


    srand(time(NULL));

    QApplication a(argc, argv);
    MainWindow w;




    if (w.startProgram)
    {
        w.show();
        return a.exec();
    }
    else
    {
        return 0;
    }
}
