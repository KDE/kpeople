/*
    Digikam persons model
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>

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


#include "digikam-persons-model.h"

#include "digikam-person-cache-item-facet.h"

#include <QPixmap>

#include <TelepathyQt/Presence>

#include <KTp/presence.h>
#include <KDebug>

DigikamPersonsModel::DigikamPersonsModel(QHash<QUrl, DigikamPersonCacheItemFacet*> *data, QObject *parent)
    : QAbstractListModel(parent)
{
    m_data = data;
}

DigikamPersonsModel::~DigikamPersonsModel()
{

}

QVariant DigikamPersonsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole:
            return m_data->values().at(index.row())->label();
        case DigikamPersonsModel::AccountIdRole:
            return m_data->values().at(index.row())->accountId();
        case DigikamPersonsModel::ContactIdRole:
            return m_data->values().at(index.row())->contactId();
        case DigikamPersonsModel::PresenceIconRole: //IMPersonsModel::PresenceIconRole
            QPixmap presencePixmap;
            Tp::ConnectionPresenceType status = (Tp::ConnectionPresenceType)m_data->values().at(index.row())->imStatusType();

            KTp::Presence presence(Tp::Presence(status, QString(), QString()));
            presencePixmap = presence.icon().pixmap(16, 16);
            return QVariant::fromValue(presencePixmap);
    }

    return QVariant();
}

int DigikamPersonsModel::rowCount(const QModelIndex &parent) const
{
    return m_data->size();
}

void DigikamPersonsModel::beginInsertData(const QModelIndex &index, int first, int last)
{
    beginInsertRows(index, first, last);
}

void DigikamPersonsModel::endInsertData()
{
    endInsertRows();
}

QUrl DigikamPersonsModel::queryPersonByName(const QString& name)
{
    Q_FOREACH (DigikamPersonCacheItemFacet *person, m_data->values()) {
        if (person->label() == name) {
            return person->uri();
        }
    }

    return QUrl();
}

QPixmap DigikamPersonsModel::presencePixmapForUri(const QUrl& uri)
{
    kDebug();
    Q_FOREACH (DigikamPersonCacheItemFacet *person, m_data->values()) {
        if (person->uri() == uri) {
            QPixmap presencePixmap;
            Tp::ConnectionPresenceType status = (Tp::ConnectionPresenceType)person->imStatusType();
            kDebug() << "Found person by uri, presence is" << status;
            KTp::Presence presence(Tp::Presence(status, QString(), QString()));
            presencePixmap = presence.icon().pixmap(16, 16);
            return presencePixmap;
        }
    }

    return QPixmap();
}
