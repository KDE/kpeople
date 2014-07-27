/*
 * Copyright 2014  Nilesh Suthar <nileshsuthar@live.in>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "postviewdelegate.h"

QSize PostViewDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize s;
    s = QSize(option.decorationSize.width(), option.decorationSize.height());
    s.setHeight(option.decorationSize.height() * 4);
    return s;
}

void PostViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    QString text = qvariant_cast<QString>(index.data(PostTextRole));
    QString time = qvariant_cast<QString>(index.data(PostTimeRole));
    QString info = qvariant_cast<QString>(index.data(PostInfoRole));
    QString link = qvariant_cast<QString>(index.data(PostLinkRole));

    QFont textFont = QApplication::font();
    QRect textRect = option.rect;
    QRect timeRect = option.rect;
    QRect infoRect = option.rect;

    QFontMetrics fm(textFont);
    painter->setFont(textFont);
    painter->drawText(textRect, text);
    painter->drawText(timeRect, Qt::AlignRight, time);
    infoRect.setTop(textRect.top() + fm.height());
    painter->drawText(infoRect, info);
    painter->restore();

}

#include "postviewdelegate.moc"
