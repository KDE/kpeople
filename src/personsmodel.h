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
        UriRole = Qt::UserRole, ///nepomuk URI STRING
        ChildContactsUriRole, ///returns list of child contact roles STRINGLIST
        FullNamesRole, ///nco:fullname STRINGLIST
        EmailsRole, ///nco:email STRINGLIST
        NicknamesRole, ///nco:imNickName STRINGLIST
        PhonesRole, ///nco:phones STRINGLIST
        IMsRole, ///STRINGLIST
        PhotosRole, ///nie:url of the photo STRINGLIST

        PresenceTypeRole, ///QString containing most online presence type
        PresenceDisplayRole, ///QString containing displayable name for most online presence
        PresenceDecorationRole, ///KIcon displaying current presence
        PresenceIconNameRole, ///QString with icon name of the current presence

        GroupsRole, ///groups STRINGLIST

        UserRole = Qt::UserRole + 100 ///< in case it's needed to extend, use this one to start from
    };

    PersonsModel(QObject *parent = 0);

    virtual ~PersonsModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

Q_SIGNALS:
    void modelInitialized();

private Q_SLOTS:
    void onContactsFetched();

    void onContactAdded(const QString &contactId);
    void onContactChanged(const QString &contactId);
    void onContactRemoved(const QString &contactId);

    void onAddContactToPerson(const QString &contactId, const QString &personId);
    

private:
    Q_DISABLE_COPY(PersonsModel)

    //methods that manipulate the model
    void addPerson(const MetaContact &mc);
    void removePerson(const QString &id);

//     /**
//      * Adds new contact to the model with @param uri as its URI
//      */
//     void addContact(const QUrl &uri);
//
//     /**
//      * Refreshes data of the contact given by @param uri
//      */
//     void updateContact(const QUrl &uri);
//
//     /* Removes contact with @param uri from the model
//      */
//     void removeContact(const QUrl &uri);
//
//     /**
//      * Adds new person to the model with @param uri as its URI
//      */
//     void addPerson(const QUrl &uri);
//
//     /**
//      * Removes person with @param uri from the model (not Nepomuk)
//      */
//     void removePerson(const QUrl &uri);
//
//     /**
//      * Adds contacts to existing PIMO:Person
//      */
//     void addContactsToPerson(const QUrl &personUri, const QList<QUrl> &contacts);
//
//     /**
//      * Removes given contacts from existing PIMO:Person
//      */
//     void removeContactsFromPerson(const QUrl &personUri, const QList<QUrl> &contacts);

    PersonsModelPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(PersonsModel);
};
}


#endif // PERSONS_MODEL_H
