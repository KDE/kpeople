/*
    Persons Model
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


#ifndef PERSONS_MODEL_H
#define PERSONS_MODEL_H

#include "kpeople_export.h"

#include <QModelIndex>
#include <QUrl>
#include <QStandardItemModel>

class PersonsModelItem;
class PersonsModelContactItem;
class KPEOPLE_EXPORT PersonsModel : public QStandardItemModel
{
    Q_OBJECT
    Q_DISABLE_COPY(PersonsModel)

public:
    enum ContactType {
        Email,
        IM,
        Phone,
        MobilePhone,
        Postal
    };

    enum Role {
        ContactTypeRole = Qt::UserRole,
        ContactIdRole,
        UriRole
    };

    explicit PersonsModel(QObject *parent = 0);

private slots:
    void init(const QHash< QUrl, PersonsModelItem* >& personNodes, const QHash< PersonsModelItem*, QList< PersonsModelContactItem* > >& contactNodes);

private:
//     PersonsModelPrivate * const d_ptr;
//     Q_DECLARE_PRIVATE(PersonsModel)
};

#endif // PERSONS_MODEL_H
