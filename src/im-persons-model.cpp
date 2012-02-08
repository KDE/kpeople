/*
    IM Persons model
    Copyright (C) 2011  Martin Klapetek <martin.klapetek@gmail.com>

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


#include "im-persons-model.h"
#include "person-cache-item-set.h"
#include "im-person-cache-item-facet.h"

#include <QPixmap>

#include <KDebug>
#include <KIcon>

#include <TelepathyQt/Presence>
#include <KTp/presence.h>

IMPersonsModel::IMPersonsModel(QHash<QUrl, IMPersonCacheItemFacet*> *data, QObject *parent)
    : QAbstractItemModel(parent)
{
    m_data = data;
}

IMPersonsModel::~IMPersonsModel()
{

}

QVariant IMPersonsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole:
            return m_data->values().at(index.row())->label();
        case IMPersonsModel::AccountIdRole:
            return m_data->values().at(index.row())->accountId();
        case IMPersonsModel::ContactIdRole:
            return m_data->values().at(index.row())->contactId();
        case Qt::DecorationRole: //IMPersonsModel::PresenceIconRole
            QPixmap presencePixmap;
            Tp::ConnectionPresenceType status = (Tp::ConnectionPresenceType)m_data->values().at(index.row())->imStatusType();

            KTp::Presence presence(Tp::Presence(status, QString(), QString()));
            presencePixmap = presence.icon().pixmap(16, 16);
            return QVariant::fromValue(presencePixmap);
    }

    return QVariant();
}

int IMPersonsModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

int IMPersonsModel::rowCount(const QModelIndex& parent) const
{
    return m_data->size();
}

QModelIndex IMPersonsModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}

QModelIndex IMPersonsModel::index(int row, int column, const QModelIndex& parent) const
{
    if (column > 1 || row >= rowCount()) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

void IMPersonsModel::beginInsertData(const QModelIndex &index, int first, int last)
{
    beginInsertRows(index, first, last);
}

void IMPersonsModel::endInsertData()
{
    endInsertRows();
}
