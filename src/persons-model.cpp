/*
    Persons Model
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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


#include "persons-model.h"
#include "persons-model-item.h"
#include "persons-model-contact-item.h"
#include "person-cache.h"
#include <Nepomuk/Vocabulary/NCO>
#include <KDebug>

// class PersonsModelPrivate {
// public:
// };

PersonsModel::PersonsModel(QObject *parent, bool init)
    : QStandardItemModel(parent)
//     , d_ptr(new PersonsModelPrivate)
{
    if(init) {
        PersonCache* cache = new PersonCache(this);
    
        connect(cache,
                SIGNAL(contactsFetched(QList<PersonsModelItem*>,QList<PersonsModelContactItem*>)),
                SLOT(init(QList<PersonsModelItem*>,QList<PersonsModelContactItem*>)));
        cache->startQuery();
    }
}

template <class T>
QList<QStandardItem*> toStandardItems(const QList<T*>& items)
{
    QList<QStandardItem*> ret;
    foreach(QStandardItem* it, items) {
        ret += it;
    }
    return ret;
}

void PersonsModel::init(const QList<PersonsModelItem*>& people, const QList<PersonsModelContactItem*>& other)
{
    QStandardItem* root = invisibleRootItem();
    root->appendRows(toStandardItems(people));
    root->appendRows(toStandardItems(other));
}
