/*                                                                         *
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

#ifndef TELEPATHY_TREE_NODE_H
#define TELEPATHY_TREE_NODE_H

#include <QObject>
#include <QVariant>

#include <KTp/ktp-export.h>

class KTP_EXPORT TreeNode : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(TreeNode)

public:
    TreeNode();
    virtual ~TreeNode();
    TreeNode *childAt(int index) const;
    void addChild(TreeNode *node);
    int indexOf(TreeNode *node) const;
    int size() const;
    TreeNode *parent() const;
    virtual QVariant data(int role) const;
    virtual bool setData(int role, const QVariant &value);

public Q_SLOTS:
    virtual void remove();

Q_SIGNALS:
    void changed(TreeNode *);
    void childrenAdded(TreeNode *parent, const QList<TreeNode *> &nodes);
    void childrenRemoved(TreeNode *parent, int first, int last);

private:
    struct Private;
    friend struct Private;
    Private *mPriv;
};

#endif // TELEPATHY_TREE_NODE_H
