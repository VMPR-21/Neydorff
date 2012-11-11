#include "ClipCopyPaste.h"

ClipCopyPaste::ClipCopyPaste()
{
}

void ClipCopyPaste::clipCopy(QTableWidget *table)
{
    QClipboard *clipboard = QApplication::clipboard();
    QTableWidgetItem *tmp_cell;
    QString table_cells = "";
    QTableWidgetSelectionRange range;
    //    QList <QTableWidgetSelectionRange> ranges = ui->tableWidget_Znach->selectedRanges();
    QList <QTableWidgetSelectionRange> ranges = table->selectedRanges();

    if (ranges.isEmpty())
    {
        range = QTableWidgetSelectionRange();
    }
    else
    {
        range = ranges.first();
    }

    for (int i = 0; i < range.rowCount(); i++)
    {
        if (i > 0)
        {
            table_cells += "\n";
        }

        for (int j = 0; j < range.columnCount(); j++)
        {
            if (j > 0)
            {
                table_cells += "\t";
            }

            //            tmp_cell = ui->tableWidget_Znach->item(range.topRow()+i,range.leftColumn()+j);
            tmp_cell = table->item(range.topRow() + i, range.leftColumn() + j);

            if (tmp_cell)
            {
                table_cells += tmp_cell->text();
            }
            else
            {
                table_cells += "";
            }
        }
    }

    // NEW NEW 24.01 NEW NEW NEW //
    table_cells += "\n";
    // NEW NEW 24.01 NEW NEW NEW //
    clipboard->setText(table_cells); // заполнение буфера обмена
}

void ClipCopyPaste::clipPaste(QTableWidget *table)
{
    QClipboard *clipboard = QApplication::clipboard();
    QString table_cells = clipboard->text();
    QTableWidgetSelectionRange range;
    //    QList <QTableWidgetSelectionRange> ranges = ui->tableWidget_Znach->selectedRanges();
    QList <QTableWidgetSelectionRange> ranges = table->selectedRanges();

    if (ranges.isEmpty())
    {
        range = QTableWidgetSelectionRange();
    }
    else
    {
        range = ranges.first();
    }

    int i, row, col;
    QString number = "";
    i = row = col = 0;
    // NEW NEW 24.01 NEW NEW NEW //
    QTableWidgetItem *tmp_cell;

    // NEW NEW 24.01 NEW NEW NEW //
    for (i = 0; i < table_cells.size(); i++)
    {
        if(table_cells[i] == '\t')
        {
            // NEW NEW 24.01 NEW NEW NEW //
            tmp_cell = table->item(range.topRow() + row, range.leftColumn() + col);

            if(!tmp_cell)
                break;

            // NEW NEW 24.01 NEW NEW NEW //
            table->item(range.topRow() + row, range.leftColumn() + col)->setText(number);
            number = "";
            col++;
        }
        else if(table_cells[i] == '\n')
        {
            // NEW NEW 24.01 NEW NEW NEW //
            tmp_cell = table->item(range.topRow() + row, range.leftColumn() + col);

            if(!tmp_cell)
                break;

            // NEW NEW 24.01 NEW NEW NEW //
            table->item(range.topRow() + row, range.leftColumn() + col)->setText(number);
            number = "";
            col = 0;
            row++;
        }
        else
        {
            //            ui->tableWidget_Znach->item(range.topRow()+row,range.leftColumn()+col)->setText(table_cells.at(i));
            number += table_cells.at(i);
        }
    }
}
