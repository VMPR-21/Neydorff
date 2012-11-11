#include <QApplication>
#include <QThread>
#include <QTextCodec>
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
