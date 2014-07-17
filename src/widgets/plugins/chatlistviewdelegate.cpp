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

#include "chatlistviewdelegate.h"

ChatListviewDelegate::ChatListviewDelegate()
{
}

void ChatListviewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                 const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    QFont font = QApplication::font();
    QFont TimeFont = QApplication::font();
    QFont Messagefont = QApplication::font();
    TimeFont.setItalic(true);
    font.setBold(true);
    QFontMetrics fm(font);

    QString senderAlias = qvariant_cast<QString>(index.data(senderAliasRole));
    QString message = qvariant_cast<QString>(index.data(messageRole));
    QString time = qvariant_cast<QString>(index.data(messageTimeRole));


    QRect AliasRect = option.rect;
    QRect MessageRect = option.rect;
    QRect TimeRect = option.rect;
    MessageRect.setLeft(fm.width(senderAlias) + 10);
    MessageRect.setHeight(MessageRect.height() * 2);


    painter->setFont(font);
    painter->drawText(AliasRect, senderAlias + ":");

    painter->setFont(Messagefont);
    painter->drawText(MessageRect, message);

    painter->setFont(TimeFont);
    painter->drawText(TimeRect, Qt::AlignRight, time);

    painter->restore();

}

#include "chatlistviewdelegate.moc"
