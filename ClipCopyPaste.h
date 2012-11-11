#ifndef CLIPCOPYPASTE_H
#define CLIPCOPYPASTE_H

#include <QTableWidget>
#include <QApplication>
#include <QKeyEvent>
#include <QClipboard>

class ClipCopyPaste
{
public:
    ClipCopyPaste();
    void clipCopy(QTableWidget *table);
    void clipPaste(QTableWidget *table);
};

#endif // CLIPCOPYPASTE_H
