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

//TODO: use proper, runtime values there
QSize MergeDelegate::s_decorationSize(SIZE_STANDARD_PIXMAP, SIZE_STANDARD_PIXMAP);
QSize MergeDelegate::s_arrowSize(15, 15);

QSize MergeDelegate::pictureSize()
{
    return s_decorationSize;
}

MergeDelegate::MergeDelegate(QAbstractItemView *parent)
    : KExtendableItemDelegate(parent)
{
    static QIcon arrowD = QIcon::fromTheme(QStringLiteral("arrow-down"));
    setContractPixmap(arrowD.pixmap(s_arrowSize));

    static QIcon arrowR = QIcon::fromTheme(QStringLiteral("arrow-right"));
    setExtendPixmap(arrowR.pixmap(s_arrowSize));
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
    if (!old.indexes().isEmpty()) {
        QModelIndex oldIdx = old.indexes().first();

        if (isExtended(oldIdx)) {
            contractItem(oldIdx);
        }
    }
    if (!now.indexes().isEmpty()) {
        QModelIndex idx = now.indexes().first();
        extendItem(buildMultipleLineLabel(idx), idx);
    }
}

QWidget *MergeDelegate::buildMultipleLineLabel(const QModelIndex &idx)
{
    QString contents;
    int rows = idx.model()->rowCount(idx);
    for (int i = 0 ; i < rows; ++i) {
        QModelIndex child = idx.child(i, 0);
        Match m = child.data(MergeDialog::MergeReasonRole).value<Match>();

        QString name = m.indexB.data(Qt::DisplayRole).toString();
        QString display = i18nc("name: merge reasons", "%1: %2", name, m.matchReasons().join(i18nc("reasons join", ", ")));
        contents += display + QLatin1String("<p/>");
    }
    QLabel *childDisplay = new QLabel(contents, dynamic_cast<QWidget *>(parent()));
    childDisplay->setAlignment(Qt::AlignRight);
    childDisplay->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    return childDisplay;
}

void MergeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &optionOld, const QModelIndex &index) const
{
    QStyleOptionViewItem option(optionOld);
    QStyleOptionViewItemV4 opt(option);
    KExtendableItemDelegate::paint(painter, option, index);

    const int separation = 5;

    int facesRows = qMin(index.model()->rowCount(index), MAX_MATCHING_CONTACTS_ICON);
    for (int i = 0; i < facesRows; i++) { // Children Icon Displaying Loop
        const QModelIndex child = index.child(i, 0);

        QVariant decoration = child.data(Qt::DecorationRole);
        Q_ASSERT(decoration.type() == (QVariant::Icon));

        QIcon pix = decoration.value<QIcon>();
        QPoint pixmapPoint = {option.rect.width() / 2 + i *(s_decorationSize.width() + separation), option.rect.top()};
        painter->drawPixmap(pixmapPoint, pix.pixmap(s_decorationSize));
    }
    // draw a vertical line to separate the original person and the merging contacts
    int midWidth = option.rect.width() / 2;
    painter->setPen(opt.palette.color(QPalette::Background));
    painter->drawLine(option.rect.left() + midWidth - SIZE_STANDARD_PIXMAP, option.rect.bottom() - 5,
                      option.rect.left() + midWidth - SIZE_STANDARD_PIXMAP, option.rect.top() + 5);
}
