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

class PersonCacheItem;
class PersonsModelPrivate;
class TreeNode;


class KPEOPLE_EXPORT PersonsModel : public QAbstractItemModel
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
        ContactTypeIconNameRole,
        ContactIdRole
    };

    explicit PersonsModel(const QHash<QUrl, TreeNode *> &personNodes,
                          const QHash<TreeNode *, QList<TreeNode *> > &contactNodes,
                          QObject *parent = 0);
    virtual ~PersonsModel();

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex index(TreeNode *node) const;

public Q_SLOTS:
    void onItemChanged(TreeNode *node);
    void onItemsAdded(TreeNode *parent, const QList<TreeNode *> &nodes);
    void onItemsRemoved(TreeNode *parent, int first, int last);

private:
    PersonsModelPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(PersonsModel)
};

#endif // PERSONS_MODEL_H
