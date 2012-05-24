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
#include "tree-node.h"
#include "person-cache-item.h"
#include "persons-model-item.h"
#include "persons-model-contact-item.h"
#include <Nepomuk/Vocabulary/NCO>
#include <KDebug>

class PersonsModelPrivate {
public:
    TreeNode *m_tree;
//     QHash<QUrl, PersonCacheItem*> *data;

    TreeNode *node(const QModelIndex &index) const;
};

TreeNode *PersonsModelPrivate::node(const QModelIndex &index) const
{
    TreeNode *node = reinterpret_cast<TreeNode *>(index.internalPointer());
    return node ? node : m_tree;
}

//-----------------------------------------------------------------------------

PersonsModel::PersonsModel(QHash<QUrl, PersonCacheItem*> *data, QObject *parent)
    : QAbstractItemModel(parent),
      d_ptr(new PersonsModelPrivate)
{
    Q_D(PersonsModel);

    d->m_tree = new TreeNode;

    connect(d->m_tree,
            SIGNAL(changed(TreeNode*)),
            SLOT(onItemChanged(TreeNode*)));

    connect(d->m_tree,
            SIGNAL(childrenAdded(TreeNode*,QList<TreeNode*>)),
            SLOT(onItemsAdded(TreeNode*,QList<TreeNode*>)));

    connect(d->m_tree,
            SIGNAL(childrenRemoved(TreeNode*,int,int)),
            SLOT(onItemsRemoved(TreeNode*,int,int)));

    QList<TreeNode *> personNodes;
    QHash<TreeNode *, QList<TreeNode *> > contactNodes;
    Q_FOREACH(PersonCacheItem *person, data->values()) {
        personNodes << new PersonsModelItem(person);
        kDebug() << person->data(Nepomuk::Vocabulary::NCO::imID());
        if (!person->data(Nepomuk::Vocabulary::NCO::imID()).isEmpty()) {
            contactNodes.insert(personNodes.last(), QList<TreeNode *>() << new PersonsModelContactItem(person->data(Nepomuk::Vocabulary::NCO::imNickname()),
                person->data(Nepomuk::Vocabulary::NCO::imID()),
                PersonsModel::IM));
        }
        if (!person->data(Nepomuk::Vocabulary::NCO::hasEmailAddress()).isEmpty()) {
            //FIXME: put proper email here
            contactNodes.insert(personNodes.last(), QList<TreeNode *>() << new PersonsModelContactItem(QLatin1String("Email"),
                                                                                                       QLatin1String("Email"),
                                                                                                       PersonsModel::Email));
        }
        //d->m_tree->childAt(d->m_tree->size() - 1)->addChild(new PersonsModelContactItem());
    }

    onItemsAdded(d->m_tree, personNodes);

    Q_FOREACH(TreeNode *node, contactNodes.keys()) {
        onItemsAdded(node, contactNodes.value(node));
    }
}

PersonsModel::~PersonsModel()
{
    Q_D(PersonsModel);

    delete d->m_tree;
}

void PersonsModel::onItemsAdded(TreeNode *parent, const QList<TreeNode *> &nodes)
{
    QModelIndex parentIndex = index(parent);
    int currentSize = rowCount(parentIndex);
    beginInsertRows(parentIndex, currentSize, currentSize + nodes.size() - 1);
    Q_FOREACH (TreeNode *node, nodes) {
        parent->addChild(node);
    }
    endInsertRows();
}

void PersonsModel::onItemsRemoved(TreeNode *parent, int first, int last)
{
    QModelIndex parentIndex = index(parent);
    QList<TreeNode *> removedItems;
    beginRemoveRows(parentIndex, first, last);
    for (int i = last; i >= first; i--) {
        parent->childAt(i)->remove();
    }
    endRemoveRows();
}

void PersonsModel::onItemChanged(TreeNode* node)
{

}

QVariant PersonsModel::data(const QModelIndex& index, int role) const
{
    Q_D(const PersonsModel);
    if (!index.isValid()) {
        return QVariant();
    }

    return d->node(index)->data(role);
}

int PersonsModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

int PersonsModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const PersonsModel);
    return d->node(parent)->size();
}

QModelIndex PersonsModel::parent(const QModelIndex &index) const
{
    Q_D(const PersonsModel);
    if (!index.isValid()) {
        return QModelIndex();
    }

    TreeNode *currentNode = d->node(index);
    if (currentNode->parent()) {
        return PersonsModel::index(currentNode->parent());
    } else {
        // no parent: return root node
        return QModelIndex();
    }
}

QModelIndex PersonsModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_D(const PersonsModel);
    TreeNode *parentNode = d->node(parent);
    Q_ASSERT(parentNode);
    if (row >= 0 && row < parentNode->size() && column == 0) {
        return createIndex(row, column, parentNode->childAt(row));
    }

    return QModelIndex();
}

QModelIndex PersonsModel::index(TreeNode *node) const
{
    if (node->parent()) {
        return createIndex(node->parent()->indexOf(node), 0, node);
    } else {
        return QModelIndex();
    }
}