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

#include <kpeople/widgets/kpeoplewidgets_export.h>

#include <QItemSelection>
#include <QItemDelegate>
#include <KExtendableItemDelegate>

class QAbstractItemView;
class QItemSelection;

class KPEOPLEWIDGETS_EXPORT MergeDelegate: public KExtendableItemDelegate
{
    Q_OBJECT

public:
    explicit MergeDelegate(QAbstractItemView *parent);
    ~MergeDelegate();

    static QSize pictureSize();

public Q_SLOTS:
    void onClickContactParent(const QModelIndex &parent);
    void onSelectedContactsChanged(const QItemSelection &now, const QItemSelection &old);

private:

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)const Q_DECL_OVERRIDE;
    QWidget *buildExtensionWidget(const QModelIndex &idx);
    QWidget *buildMultipleLineLabel(const QModelIndex &idx);

    static QSize s_arrowSize;
    static QSize s_decorationSize;
};

#endif // MERGEDELEGATE_H
