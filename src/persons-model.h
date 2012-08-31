/*
    Persons Model
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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


#ifndef PERSONS_MODEL_H
#define PERSONS_MODEL_H

#include "kpeople_export.h"

#include <QModelIndex>
#include <QUrl>
#include <QStandardItemModel>

class KJob;
class PersonsModelItem;
class PersonsModelContactItem;

class KPEOPLE_EXPORT PersonsModel : public QStandardItemModel
{
    Q_OBJECT
    Q_DISABLE_COPY(PersonsModel)

public:
    enum ContactType {
        Email,
        IM,
        Phone,
        MobilePhone,
        Postal
    };

    enum Role {
        ContactTypeRole = Qt::UserRole,
        ContactIdRole,
        UriRole,
        NameRole,
        EmailRole,
        NickRole,
        PhoneRole,
        IMRole,
        PhotoRole
    };

    /**
     * @p initialize set it to false if you don't want it to use nepomuk values
     *               useful for unit testing.
     */
    explicit PersonsModel(QObject *parent = 0, bool initialize = true);

    /**
     * The @p contactUri will be removed from @p personUri and it will be added to a new 
     * empty pimo:Person instance.
     */
    Q_SCRIPTABLE void unmerge(const QUrl& contactUri, const QUrl& personUri);

public slots:
    void init(const QList<PersonsModelItem*>& people, const QList<PersonsModelContactItem*>& other);

private slots:
    void unmergeFinished(KJob*);

signals:
    void peopleAdded();

private:
//     PersonsModelPrivate * const d_ptr;
//     Q_DECLARE_PRIVATE(PersonsModel)
};

#endif // PERSONS_MODEL_H
