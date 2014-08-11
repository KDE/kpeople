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

#include "emaillistmodel.h"

#include <QDebug>
#include <KGlobal>
#include <KLocale>

EmailListModel::EmailListModel(QObject *parent) : QAbstractListModel(parent)
{
}

EmailListModel::EmailListModel(QList<email> list, QObject *parent) : QAbstractListModel(parent)
{
    emailList = list;
};

int EmailListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return emailList.count();
}

QVariant EmailListModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();

    struct email mail = emailList.at(index.row());
    QString desc = mail.desc;
    QString subject = mail.subject;
    QString dateString = KGlobal::locale()->formatDateTime(mail.date.dateTime(), KLocale::FancyShortDate, KLocale::Seconds);

    if (mail.subject == "")
        subject = "No Subject";

    subject.replace('\n', ' ');
    desc.replace('\n', ' ');
    desc.truncate(160);
    desc.append("...");

    if (role == mailSubjectRole) {
        return subject;
    } else if (role == mailDescRole) {
        return desc;
    } else if (role == mailTimeRole) {
        return dateString;
    }

    return QVariant();
}

void EmailListModel::addEmail(email mail)
{
    beginInsertRows(QModelIndex(), emailList.count(), emailList.count() + 1);
    emailList << mail;
    endInsertRows();
}

QUrl EmailListModel::getItemUrl(int row)
{
    return emailList.at(row).url;
}
