/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2014  <copyright holder> <email>
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

#ifndef EMAILLISTMODEL_H
#define EMAILLISTMODEL_H

#include <QAbstractListModel>
#include <KDateTime>
#include <QUrl>

struct email {
    QString subject;
    QString desc;
    KDateTime date;
    QUrl url;
};


class EmailListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    EmailListModel(QObject *parent=0);
    EmailListModel(QList<email> list, QObject *parent=0);

    virtual QVariant data(const QModelIndex& index, int role) const;

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual int rowCount(const QModelIndex& parent) const;

    void addEmail(struct email mail);
    QUrl getItemUrl(int row);



private:
    QList<email> emailList;

};

#endif // EMAILLISTMODEL_H
