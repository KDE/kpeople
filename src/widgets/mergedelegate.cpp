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

#include "mergedelegate.h"
#include "personsmodel.h"
#include "mergedialog.h"
#include <match.h>

#include <QPainter>
#include <QPixmap>

#include <QTreeView>
#include <KIcon>

#include <QStyleOptionViewItem>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QModelIndex>

#include <KStandardDirs>
#include <QApplication>

#include <KDebug>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QItemSelection>
#include <QStylePainter>

#define MAX_MATCHING_CONTACTS_ICON 5

using namespace KPeople;

MergeDelegate::MergeDelegate(QAbstractItemView *parent)
    : KExtendableItemDelegate(parent)
    , m_arrowSize(15,15)
    , m_defaultPixmap(KStandardDirs::locate("data", "kpeople/dummy_avatar.png"))
{
    m_defaultPixmap = m_defaultPixmap.scaled(QSize(35, 35), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
}

MergeDelegate::~MergeDelegate()
{}

void MergeDelegate::onClickContactParent(const QModelIndex &parent)
{
    if (isExtended(parent)) {
        contractItem(parent);
    } else {
        QItemSelection item = QItemSelection(parent, parent);
        onSelectedContactsChanged(item, QItemSelection());
    }
}

void MergeDelegate::onSelectedContactsChanged(const QItemSelection &now , const QItemSelection &old)
{
    if(old.indexes().size()) {
        QModelIndex oldIdx = old.indexes().first();

        if (isExtended(oldIdx)) {
            contractItem(oldIdx);
        }
    }
    if (now.indexes().size()) {
        QModelIndex idx = now.indexes().first();
        extendItem(buildMultipleLineLabel(idx), idx);
    }
}

QWidget* MergeDelegate::buildMultipleLineLabel(const QModelIndex &idx)
{
    QString contents;
    int rows = idx.model()->rowCount(idx);
    for (int i=0 ; i < rows; ++i) {
        QModelIndex child = idx.child(i,0);
        Match m = child.data(MergeDialog::MergeReasonRole).value<Match>();

        QString mergeReason;
        // NOTE : could be improve to be perfectly sure we're displaying the common property
        QList<QString> mergeReasonList = m.indexB.data(m.role.first()).value<QStringList>();
        if (!mergeReasonList.isEmpty()) {
            mergeReason = mergeReasonList.first();
        }
        QString name = m.indexB.data(Qt::DisplayRole).toString();
        QString display = (name == mergeReason) ? name : name.append(" : "+mergeReason);
        contents += display+"<p/>";
    }
    QLabel *childDisplay= new QLabel(contents, dynamic_cast<QWidget*>(parent()));

    childDisplay->setAlignment(Qt::AlignRight);
    QPalette p = childDisplay->palette();
    p.setColor(QPalette::Text, p.color(QPalette::HighlightedText));
    childDisplay->setPalette(p);
    return childDisplay;
}


void MergeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &optionOld, const QModelIndex &index) const
{
    QStyleOptionViewItem option = QStyleOptionViewItem(optionOld);
    option.rect.translate(m_arrowSize.width(), 0);

    // draw the arrow to let the user know it's expandable
    QPoint arrowRect = optionOld.rect.topLeft();
    int rows = index.model()->rowCount(index);

    QPoint arrowPlace(arrowRect.x(), arrowRect.y()+m_defaultPixmap.height()/2+option.fontMetrics.height()/4);
    if (!isExtended(index)) {
        static KIcon arrow("arrow-right");
        painter->drawPixmap(arrowPlace, arrow.pixmap(m_arrowSize));
    } else {
        static KIcon arrow("arrow-down");
        painter->drawPixmap(arrowPlace, arrow.pixmap(m_arrowSize));

        // paint the extender in blue
        QStyleOptionViewItemV4 opt(option);
        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
    }

    KExtendableItemDelegate::paint(painter, option, index);

    const int separation = 5;
    int facesRows = qMin(rows, MAX_MATCHING_CONTACTS_ICON );
    for (int i=0; i < facesRows; i++) { // Children Icon Displaying Loop
        const QModelIndex child = index.child(i,0);
        QPixmap icon = qvariant_cast<QPixmap>(child.data(Qt::DecorationRole));

        if (!icon.isNull()) {
            icon = icon.scaled(m_defaultPixmap.size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        } else {
            icon = m_defaultPixmap;
        }
        QPoint pixmapRect;
        pixmapRect.setX(option.rect.width()/2 + i*(icon.width() + separation));
        pixmapRect.setY(option.rect.top() + m_defaultPixmap.height()/4+option.fontMetrics.height()/4);
        painter->drawPixmap(pixmapRect, icon);
    }
}

QSize MergeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // the width doesn't matter here : the dialog overchoose it.
    QSize defaultSize = KExtendableItemDelegate::sizeHint(option, index);
    defaultSize.rheight() += m_defaultPixmap.height();
    return defaultSize;
}

