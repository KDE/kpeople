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


#include "persons-model.h"
#include "person-cache-item.h"
#include "persons-model-item.h"
#include "persons-model-contact-item.h"
#include "person-cache.h"
#include <Nepomuk/Vocabulary/NCO>
#include <KDebug>

class PersonsModelPrivate {
public:
};

PersonsModel::PersonsModel(QObject *parent)
    : QStandardItemModel(parent)
//     , d_ptr(new PersonsModelPrivate)
{
    PersonCache* cache = PersonCache::instance();
  
    connect(cache,
            SIGNAL(contactsFetched(QHash<QUrl,QStandardItem*>,QHash<QStandardItem*,QList<QStandardItem*> >)),
            SLOT(init(QHash<QUrl,QStandardItem*>,QHash<QStandardItem*,QList<QStandardItem*> >)));
    cache->startQuery();
}

void PersonsModel::init(const QHash< QUrl, QStandardItem* >& personNodes, const QHash< QStandardItem*, QList< QStandardItem* > >& contactNodes)
{
    QStandardItem* root = invisibleRootItem();
    root->appendRows(personNodes.values());

    Q_FOREACH(QStandardItem *node, contactNodes.keys()) {
        if (node == 0) {
            root->appendRows(contactNodes.value(node));
        } else {
            node->appendRows(contactNodes.value(node));
        }
    }
}
