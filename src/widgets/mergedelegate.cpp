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
#include <match_p.h>

#include <QPainter>
#include <QPixmap>
#include <QStyleOptionViewItem>
#include <QAbstractItemView>
#include <QModelIndex>
#include <QApplication>
#include <QLabel>
#include <QCheckBox>
#include <QItemSelection>
#include <QDebug>
#include <QIcon>
#include <KLocalizedString>

#define MAX_MATCHING_CONTACTS_ICON 5
#define SIZE_STANDARD_PIXMAP 35

using namespace KPeople;

MergeDelegate::MergeDelegate(QAbstractItemView *parent)
    : KExtendableItemDelegate(parent)
    , m_arrowSize(15,15)
    , m_decorationSize(SIZE_STANDARD_PIXMAP, SIZE_STANDARD_PIXMAP)
{
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
    if (old.indexes().size()) {
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
    for (int i = 0 ; i < rows; ++i) {
        QModelIndex child = idx.child(i,0);
        Match m = child.data(MergeDialog::MergeReasonRole).value<Match>();

        QString name = m.indexB.data(Qt::DisplayRole).toString();
        QString display = name + QStringLiteral(" : ") + m.matchReasons().join(i18nc("reasons join", ", "));
        contents += display+ QLatin1String("<p/>");
    }
    QLabel *childDisplay = new QLabel(contents, dynamic_cast<QWidget*>(parent()));

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

    QStyleOptionViewItemV4 opt(option);
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    QPoint arrowPlace(arrowRect.x(), arrowRect.y() + m_decorationSize.height()/2 + option.fontMetrics.height()/4);
    if (!isExtended(index)) {
        static QIcon arrow = QIcon::fromTheme(QStringLiteral("arrow-right"));
        painter->drawPixmap(arrowPlace, arrow.pixmap(m_arrowSize));
    } else {
        static QIcon arrow = QIcon::fromTheme(QStringLiteral("arrow-down"));
        painter->drawPixmap(arrowPlace, arrow.pixmap(m_arrowSize));

        // paint the extender in blue
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
    }

    option.decorationSize = QSize(SIZE_STANDARD_PIXMAP,SIZE_STANDARD_PIXMAP);
    KExtendableItemDelegate::paint(painter, option, index);

    const int separation = 5;
    static QIcon arrow = QIcon::fromTheme(QStringLiteral("arrow-right"));

    int facesRows = qMin(rows, MAX_MATCHING_CONTACTS_ICON );
    for (int i = 0; i < facesRows; i++) { // Children Icon Displaying Loop

        QPixmap pix;
        const QModelIndex child = index.child(i,0);

        QVariant decoration = child.data(Qt::DecorationRole);
        if (decoration.type() == (QVariant::Icon)) {
            QIcon icon = decoration.value<QIcon>();
            pix = icon.pixmap(m_decorationSize);
        } else if (decoration.type() == (QVariant::Pixmap)) {
            pix = decoration.value<QPixmap>();
        }

        QPoint pixmapRect;
        pixmapRect.setX(option.rect.width()/2 + i*(m_decorationSize.width() + separation));
        pixmapRect.setY(option.rect.top() + m_decorationSize.height()/4 + option.fontMetrics.height()/4);
        painter->drawPixmap(pixmapRect, pix);
    }
    // draw a vertical blue line to separate the original person and the merging contacts
    int midWidth = option.rect.width()/2;
    painter->setPen(opt.palette.color(QPalette::Background));
    painter->drawLine( option.rect.left()+midWidth -SIZE_STANDARD_PIXMAP, option.rect.bottom()-5,
                       option.rect.left()+midWidth-SIZE_STANDARD_PIXMAP, option.rect.top()+5);
}

QSize MergeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // the width doesn't matter here : the dialog overchoose it.
    QSize defaultSize = KExtendableItemDelegate::sizeHint(option, index);
    defaultSize.rheight() += m_decorationSize.height();
    return defaultSize;
}
