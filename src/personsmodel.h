/*
    Persons Model
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

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

#include <QAbstractListModel>

#include <KABC/AddresseeList>

namespace KPeople
{
class PersonsModelFeature;
class ContactItem;
class PersonItem;
class MetaContact;
struct PersonsModelPrivate;


class KPEOPLE_EXPORT PersonsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Role {
        FormattedNameRole = Qt::DisplayRole,//QString best name for this person
        PhotoRole = Qt::DecorationRole, //QPixmap best photo for this person
        PersonIdRole = Qt::UserRole, //QString ID of this person
        PersonVCardRole, //KABC::Addressee
        ContactsVCardRole, //KABC::AddresseeList (FIXME or map?)

        GroupsRole, ///groups QStringList

        UserRole = Qt::UserRole + 0x1000 ///< in case it's needed to extend, use this one to start from
    };

    PersonsModel(QObject *parent = 0);

    virtual ~PersonsModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

Q_SIGNALS:
    void modelInitialized();

private Q_SLOTS:
    void onContactsFetched();

    //update when a resource signals a contact has changed
    void onContactAdded(const QString &contactId);
    void onContactChanged(const QString &contactId);
    void onContactRemoved(const QString &contactId);

    //update on metadata changes
    void onAddContactToPerson(const QString &contactId, const QString &newPersonId);
    void onRemoveContactsFromPerson(const QString &contactId);
    

private:
    Q_DISABLE_COPY(PersonsModel)

    //methods that manipulate the model
    void addPerson(const MetaContact &mc);
    void removePerson(const QString &id);

    QString personIdForContact(const QString &contactId);

    PersonsModelPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(PersonsModel);
};
}

// Q_DECLARE_METATYPE(KABC::Addressee)
Q_DECLARE_METATYPE(KABC::AddresseeList)

#endif // PERSONS_MODEL_H
