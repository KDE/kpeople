#include "dialog.h"
#include "logmodel.h"
#include <persondata.h>
#include <personpluginmanager.h>
#include "ui_dialog.h"
#include <QMenu>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->tableView->setModel(new LogModel(this));
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onCustomContextMenuRequested(QPoint)));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::onCustomContextMenuRequested(const QPoint& point)
{
    const QModelIndex &index = ui->tableView->indexAt(point);
    if (!index.isValid()) {
        return;
    }

    const QString &emailAddress = index.data().toString();
    PersonData person;
    person.setContactId(emailAddress);

    QMenu menu;
    QList<QAction*> actions = PersonPluginManager::actionsForPerson(&person, &menu);
    if (actions.size()) {
        menu.addActions(actions);
        menu.exec(ui->tableView->mapToGlobal(point));
    }
}
