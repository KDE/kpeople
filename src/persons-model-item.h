/*
    Persons Model Item
    Represents one person in the model
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


#ifndef PERSONS_MODEL_ITEM_H
#define PERSONS_MODEL_ITEM_H

#include "kpeople_export.h"
#include <qstandarditemmodel.h>

namespace Nepomuk2 {
class Resource;
}

class QUrl;
class PersonsModelItemPrivate;
class PersonCacheItem;

class KPEOPLE_EXPORT PersonsModelItem : public QStandardItem
{
public:
    PersonsModelItem(const QUrl &personUri);
    PersonsModelItem(const Nepomuk2::Resource &person);

    virtual QVariant data(int role) const;

    void removeContacts(const QList<QUrl> &contacts);
    void addContacts(const QList<QUrl> &contacts);
    void setContacts(const QList<QUrl> &contacts);

private:
    QVariant queryChildrenForRole(int role) const;
    QVariantList queryChildrenForRoleList(int role) const;
};

Q_DECLARE_METATYPE(PersonsModelItem*);

#endif // PERSONS_MODEL_ITEM_H
