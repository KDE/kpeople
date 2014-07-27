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

#include "eventviewdelegate.h"

QSize EventViewDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize s;
    s = QSize(option.decorationSize.width(), option.decorationSize.height());
    s.setHeight(option.decorationSize.height() * 2);
    return s;
}

void EventViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    QString name = qvariant_cast<QString>(index.data(EventNameRole));
//     QString desc = qvariant_cast<QString>(index.data(EventDescRole));
    QString date = qvariant_cast<QString>(index.data(EventDateRole));
    QString duration = qvariant_cast<QString>(index.data(EventDurationRole));
    QString location = "Location: " + qvariant_cast<QString>(index.data(EventLocationRole));
    QString link = qvariant_cast<QString>(index.data(EventLinkRole));

    QFont nameFont = QApplication::font();
    QRect nameRect = option.rect;
    QRect locationRect = option.rect;
    QRect timeRect = option.rect;
    QRect durationRect = option.rect;

    QFontMetrics fm(nameFont);
    painter->setFont(nameFont);
    painter->drawText(nameRect, name);
    
    locationRect.setTop(nameRect.top() + fm.height() + 2);
    painter->drawText(locationRect , location);
    durationRect.setTop(nameRect.top() + fm.height() + 2);
    durationRect.setLeft(fm.width(location)+5);
    painter->drawText(durationRect , "for "+ duration);
    painter->drawText(timeRect, Qt::AlignRight, date);

    painter->restore();

}

#include "eventviewdelegate.moc"
