/*
 * Tree model node
 * This file is based on TelepathyQtYell Models
 *
 * Copyright (C) 2010 Collabora Ltd. <info@collabora.co.uk>
 * Copyright (C) 2011 Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "tree-node.h"

struct TreeNode::Private
{
    Private() :
        mParent(0)
    {
    }

    ~Private()
    {
        qDeleteAll(mChildren);
        mChildren.clear();
    }

    QList<TreeNode *> mChildren;
    TreeNode *mParent;
};

TreeNode::TreeNode()
    : mPriv(new Private())
{
}

TreeNode::~TreeNode()
{

    delete mPriv;
}

TreeNode *TreeNode::childAt(int index) const
{
    return mPriv->mChildren[index];
}

void TreeNode::addChild(TreeNode *node)
{
    // takes ownership of node
    mPriv->mChildren.append(node);
    node->mPriv->mParent = this;

    // set the parent QObject so that the node doesn't get deleted if used
    // from QML/QtScript
    node->setParent(this);

    // chain changed and removed signals
    connect(node,
            SIGNAL(changed(TreeNode*)),
            SIGNAL(changed(TreeNode*)));
    connect(node,
            SIGNAL(childrenAdded(TreeNode*,QList<TreeNode*>)),
            SIGNAL(childrenAdded(TreeNode*,QList<TreeNode*>)));
    connect(node,
            SIGNAL(childrenRemoved(TreeNode*,int,int)),
            SIGNAL(childrenRemoved(TreeNode*,int,int)));
}

int TreeNode::indexOf(TreeNode *node) const
{
    return mPriv->mChildren.indexOf(node);
}

int TreeNode::size() const
{
    return mPriv->mChildren.size();
}

TreeNode *TreeNode::parent() const
{
    return mPriv->mParent;
}

QVariant TreeNode::data(int role) const
{
    Q_UNUSED(role);
    return QVariant();
}

bool TreeNode::setData(int role, const QVariant &value)
{
    Q_UNUSED(role);
    Q_UNUSED(value);
    return false;
}

void TreeNode::remove()
{
    if (mPriv->mParent) {
        disconnect(this,
                   SIGNAL(changed(TreeNode*)),
                   mPriv->mParent,
                   SIGNAL(changed(TreeNode*)));
        disconnect(this,
                   SIGNAL(childrenAdded(TreeNode*,QList<TreeNode*>)),
                   mPriv->mParent,
                   SIGNAL(childrenAdded(TreeNode*,QList<TreeNode*>)));
        disconnect(this,
                   SIGNAL(childrenRemoved(TreeNode*,int,int)),
                   mPriv->mParent,
                   SIGNAL(childrenRemoved(TreeNode*,int,int)));

        mPriv->mParent->mPriv->mChildren.removeOne(this);
    }

    deleteLater();
}

#include "tree-node.moc"
