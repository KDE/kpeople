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

#include "emaillistviewdelegate.h"
QSize EmailListViewDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize s;
    s = QSize(option.decorationSize.width(), option.decorationSize.height());
    s.setHeight(option.decorationSize.height() * 2);
    return s;
}

void EmailListViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    QString subject = qvariant_cast<QString>(index.data(mailSubjectRole));
    QString desc = qvariant_cast<QString>(index.data(mailDescRole));
    QString time = qvariant_cast<QString>(index.data(mailTimeRole));

    QFont subjectFont = QApplication::font();
    QFont timeFont = QApplication::font();
    QFont descFont = QApplication::font();
    QFontMetrics fm = QFontMetrics(subjectFont);
    timeFont.setItalic(true);

    QRect descRect = option.rect;
    QRect timeRect = option.rect;
    QRect subjectRect = option.rect;
    subjectRect.setTop(subjectRect.top() + 5);
    descRect.setTop(subjectRect.top() + fm.height());


    painter->setFont(subjectFont);
    painter->drawText(subjectRect, "Subject: " + subject);

    painter->setFont(descFont);
    painter->setPen(Qt::darkGray);
    painter->drawText(descRect, desc);

    painter->setFont(timeFont);
    painter->drawText(timeRect, Qt::AlignRight, time);

    painter->restore();

}

#include "emaillistviewdelegate.moc"
