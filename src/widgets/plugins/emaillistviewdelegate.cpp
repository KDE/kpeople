/*
    Copyright 2014  Nilesh Suthar <nileshsuthar@live.in>

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

#include "emaillistviewdelegate.h"

#include <QPainter>
#include <KLocalizedString>
#include <QApplication>

QSize EmailListViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    QSize s;
    s = QSize(option.decorationSize.width(), option.decorationSize.height());
    s.setHeight(option.decorationSize.height() * 2);
    return s;
}

void EmailListViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    QString subject = index.data(EmailListModel::SubjectRole).toString();
    QString body = index.data(EmailListModel::BodyRole).toString();
    QString time = index.data(EmailListModel::TimeRole).toString();

    QFont subjectFont = QApplication::font();
    QFont timeFont = QApplication::font();
    QFont bodyFont = QApplication::font();
    QFontMetrics fm = QFontMetrics(subjectFont);
    timeFont.setItalic(true);

    QRect bodyRect = option.rect;
    QRect timeRect = option.rect;
    QRect subjectRect = option.rect;
    subjectRect.setTop(subjectRect.top() + 5);
    bodyRect.setTop(subjectRect.top() + fm.height());

    painter->setFont(subjectFont);
    painter->drawText(subjectRect, i18n("Subject: ") + subject);

    painter->setFont(bodyFont);
    painter->setPen(Qt::darkGray);
    painter->drawText(bodyRect, fm.elidedText(body, Qt::ElideRight, bodyRect.width()));

    painter->setFont(timeFont);
    painter->drawText(timeRect, Qt::AlignRight, time);

    painter->restore();
}

#include "emaillistviewdelegate.moc"
