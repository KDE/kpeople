/*
    KPeople - Duplicates
    Copyright (C) 2013  Franck Arrecot <franck.arrecot@gmail.com>

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

#ifndef MERGEDELEGATE_H
#define MERGEDELEGATE_H

#include "../kpeople_export.h"

#include <QItemSelection>
#include <QItemDelegate>
#include <KExtendableItemDelegate>


class QAbstractItemView;
class QItemSelection;

class KPEOPLE_EXPORT MergeDelegate: public KExtendableItemDelegate
{
    Q_OBJECT

public:
    explicit MergeDelegate(QAbstractItemView *parent);
    ~MergeDelegate();

public Q_SLOTS:
    void onClickContactParent(const QModelIndex &parent);
    void onSelectedContactsChanged(const QItemSelection &now, const QItemSelection &old);

private:

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QWidget* buildExtensionWidget(const QModelIndex& idx);
    QWidget* buildMultipleLineLabel(const QModelIndex &idx);

    QSize m_arrowSize;
    QSize m_decorationSize;
};

#endif // MERGEDELEGATE_H
