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


#ifndef IM_PERSONS_MODEL_H
#define IM_PERSONS_MODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QUrl>

#include "kpeople_export.h"

class IMPersonCacheItemFacet;
class PersonCacheItem;
class PersonCacheItemSet;

class KPEOPLE_EXPORT IMPersonsModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_ENUMS(Role)

public:
    enum Role {
        // general roles
        ContactIdRole = Qt::UserRole,
        AccountIdRole,
        ConnectionStatusTypeRole,
        PresenceIconRole
    };
    IMPersonsModel(QHash<QUrl, IMPersonCacheItemFacet*> *data, QObject *parent = 0);
    ~IMPersonsModel();

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

    ///Called from Manager, so that the model can be properly updated
    void beginInsertData(const QModelIndex &index, int first, int last);
    void endInsertData();

private:
    QHash<QUrl, IMPersonCacheItemFacet*> *m_data;
};

#endif // IM_PERSONS_MODEL_H
