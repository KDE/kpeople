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

#include "emaillistmodel.h"

#include <QDebug>
#include <KGlobal>
#include <KLocale>

EmailListModel::EmailListModel(QObject *parent) : QAbstractListModel(parent)
{
}

int EmailListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return emailList.count();
}

QVariant EmailListModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid()) {
        return QVariant();
    }

    EmailItem mail = emailList.at(index.row());

    switch (role) {
    case SubjectRole:
        if (mail.subject == "") {
            mail.subject = i18n("No Subject");
        }
        mail.subject.replace('\n', ' ');
        return mail.subject;

    case BodyRole:
        mail.body.replace('\n', ' ');
        return mail.body;

    case TimeRole:
        return KGlobal::locale()->formatDateTime(mail.date.dateTime(), KLocale::FancyShortDate);
    }
    return QVariant();
}

void EmailListModel::addEmail(const EmailItem &mail)
{
    beginInsertRows(QModelIndex(), emailList.count(), emailList.count());
    emailList << mail;
    endInsertRows();
}

QUrl EmailListModel::getItemUrl(int row)
{
    return emailList.at(row).url;
}
