/*
    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "personactionsmodel.h"
#include "personsmodel.h"

#include <QPersistentModelIndex>
#include <QAction>
#include <KIcon>
#include <KLocalizedString>
#include <KToolInvocation>

#include <KTp/contact.h>
#include <KTp/actions.h>
#include <KTp/types.h>
#include <TelepathyQt/Account>

#include "personpluginmanager.h"
#include "persondata.h"

struct PersonActionsPrivate {
    PersonActionsPrivate():
        person(0)
        {}
    QList<QAction*> actions;

    QPersistentModelIndex index;
    PersonData *person;
};

PersonActionsModel::PersonActionsModel(QObject *parent)
    : QAbstractListModel(parent),
    d_ptr(new PersonActionsPrivate)
{
    Q_D(PersonActions);
}

PersonActionsModel::~PersonActionsModel()
{
    delete d_ptr;
}

void PersonActionsModel::setPerson(QAbstractItemModel* model, int row)
{
    setPerson(model->index(row, 0));
}

void PersonActionsModel::setPerson(const QPersistentModelIndex& index)
{
    Q_D(PersonActions);
    beginResetModel();
    d->actions.clear();
    d->index = index;
    if (d->person) {
        d->person->deleteLater();
    }
    d->person = new PersonData(this);
    d->person->setUri(index.data(PersonsModel::UriRole).toString());

    d->actions.append(PersonPluginManager::actionsForPerson(d->person, this));

    endResetModel();
    emit personChanged();
}

QVariant PersonActionsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const PersonActions);

    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole:
            return d->actions[index.row()]->text();
        case Qt::DecorationRole:
            return d->actions[index.row()]->icon();
        case Qt::ToolTip:
            return d->actions[index.row()]->toolTip();
    }

    return QVariant();
}

int PersonActionsModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const PersonActions);

    return parent.isValid() ? 0 : d->actions.size();
}

void PersonActionsModel::triggerAction(int row) const
{
    Q_D(const PersonActions);
    d->actions[row]->trigger();
}

QList< QAction* > PersonActionsModel::actions() const
{
    Q_D(const PersonActions);
    return d->actions;
}
