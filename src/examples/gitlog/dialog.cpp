#include "dialog.h"
#include "logmodel.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->tableView->setModel(new LogModel(this));
}

Dialog::~Dialog()
{
    delete ui;
}
