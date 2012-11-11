#include "FactorSettingsDialog.h"
#include "ui_FactorSettingsDialog.h"
#include "IFactorTable.h"

FactorSettingsDialog::FactorSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FactorSettingsDialog)
{
    ui->setupUi(this);
    invalidState = false;
    table = NULL;
    updateResult();
}

FactorSettingsDialog::~FactorSettingsDialog()
{
    delete ui;
}

bool FactorSettingsDialog::askFactorSettings(int factorIndex, const IFactorTable *table)
{
    for(int i = 0; i < table->generalFactorCount(); i++)
    {
        ui->listWidget->addItem("X" + QString::number(i + 1));
        ui->listWidget->addItem("-X" + QString::number(i + 1));
    }

    std::vector<int> oldSet = table->getGenerateRelation(factorIndex);

    for(uint i = 0; i < oldSet.size(); i++)
    {
        if(oldSet[i] > table->generalFactorCount())
            continue;

        int idx = (abs(oldSet[i]) - 1) * 2;

        if(oldSet[i] < 0) //negative means -Xi
            idx = idx + 1; //calculate index of -Xi

        ui->listWidget->selectionModel()->select(ui->listWidget->model()->index(idx, 0), QItemSelectionModel::Select);
    }

    this->_factorIndex = factorIndex;
    this->table = table;
    exec();
    return true;
}

std::vector<int> FactorSettingsDialog::getSetting()
{
    return res;
}

void FactorSettingsDialog::on_listWidget_itemSelectionChanged()
{
    updateResult();
}

void FactorSettingsDialog::updateResult()
{
    res.clear();
    ui->labelErrors->clear();
    invalidState = false;
    QString text;

    for(int i = 0; i < ui->listWidget->count(); i++)
        if(ui->listWidget->item(i)->isSelected())
        {

            int m = 1;

            if(i % 2) // now odds indexes in GUI for "-Xi"
                m *= -1;

            // else
            //     idx=-idx/2;
            //its stored as negative index.
            int idx = i / 2 + 1;
            res.push_back(idx * m);
            text += ui->listWidget->item(i)->text() + "*";
        }

    if(text.size() && text[text.size() - 1] == '*')
        text.chop(1);

    ui->resultSetLabel->setText(text);

    if(res.size() < 2)
    {
        invalidState = true;
        ui->labelErrors->setText("Ошибка: Выберите более сложную комбинацию!");
    }
    else if(table)
    {

        for(int i = 0; i < table->count() - table->generalFactorCount(); i++)
            if(i != _factorIndex && res == table->getGenerateRelation(i))
            {
                invalidState = true;
                ui->labelErrors->setText("Ошибка: повторяющаяся комбинация!");
                break;
            }
    }

    ui->labelErrors->setVisible(invalidState);
    ui->pushButtonOK->setEnabled(!invalidState);
}

void FactorSettingsDialog::on_pushButtonOK_clicked()
{
    if(!invalidState)
        close();
}

void FactorSettingsDialog::on_listWidget_itemClicked(QListWidgetItem *item)
{
}
