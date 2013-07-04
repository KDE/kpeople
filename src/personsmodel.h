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

#include <QStandardItemModel>

class KJob;
class QUrl;

namespace Nepomuk2 { class Resource; }
namespace Soprano { namespace Util { class AsyncQuery; } }

namespace KPeople
{
class PersonsModelFeature;
class ContactItem;
class PersonItem;
struct PersonsModelPrivate;


class KPEOPLE_EXPORT PersonsModel : public QStandardItemModel
{
    Q_OBJECT
    Q_DISABLE_COPY(PersonsModel)

public:
    enum Role {
        UriRole = Qt::UserRole, //nepomuk URI STRING
        ChildContactsUriRole, //returns list of child contact roles STRINGLIST
        FullNamesRole, //nco:fullname STRINGLIST
        EmailsRole, //nco:email STRINGLIST
        NicknamesRole, //nco:imNickName STRINGLIST
        PhonesRole, //nco:phones STRINGLIST
        IMsRole, //STRINGLIST
        PhotosRole, //nie:url of the photo STRINGLIST

        PresenceTypeRole, //QString containing most online presence type
        PresenceDisplayRole, //QString containing displayable name for most online presence
        PresenceDecorationRole, //KIcon displaying current presence
        PresenceIconNameRole, //QString with icon name of the current presence

        GroupsRole, //groups STRINGLIST

        UserRole = Qt::UserRole + 100 ///< in case it's needed to extend, use this one to start from
    };

    PersonsModel(QObject *parent = 0);

    virtual ~PersonsModel();

    /**Start querying the database using the supplied features*/
    void startQuery(const QList<PersonsModelFeature> &features);

    /** Creates PIMO:Person with NCO:PersonContacts as grounding occurances
     *  The list that it's passed can contain uris of both Person and PersonContacts,
     *  the method checks their type and does the right thing(tm)
     *  @param uris list of Person and PersonContact uris
     */
    Q_SCRIPTABLE static KJob* createPersonFromUris(const QList<QUrl> &uris);

    /** Removes the link between contacts and the given person
     *  @param personUri PIMO:Person uri to unlink the contacts from
     *  @param contactUris list of NCO:PersonContacts to unlink
     */
    Q_SCRIPTABLE static void unlinkContactFromPerson(const QUrl &personUri, const QList<QUrl> &contactUris);

    Q_SCRIPTABLE QModelIndex indexForUri(const QUrl &uri) const;

    Q_SCRIPTABLE QList<QModelIndex> indexesForUris(const QVariantList& uris) const;

Q_SIGNALS:
    void modelInitialized();

private Q_SLOTS:
    void jobFinished(KJob *job);
    void query(const QString &queryString);
    void nextReady(Soprano::Util::AsyncQuery *query);
    void queryFinished(Soprano::Util::AsyncQuery *query);
    void contactChanged(const QUrl &uri);
    void updateContactFinished(Soprano::Util::AsyncQuery *query);

private:
    /**
     * @return actual features used to populate the model
     */
    QList<PersonsModelFeature> modelFeatures() const;
    ContactItem* contactItemForUri(const QUrl &uri) const;
    PersonItem* personItemForUri(const QUrl &uri) const;
    QModelIndex findRecursively(int role, const QVariant &value, const QModelIndex &idx = QModelIndex()) const;

    /** Adds new contact to the model with @param uri as its URI */
    void addContact(const QUrl &uri);
    /** Refreshes data of the contact given by @param uri */
    void updateContact(const QUrl &uri);
    /** Convenience function */
    void updateContact(ContactItem *contact);
    /** Removes contact with @param uri from the model (not Nepomuk) */
    void removeContact(const QUrl &uri);

    /** Adds new person to the model with @param uri as its URI */
    void addPerson(const QUrl &uri);
    /** Removes person with @param uri from the model (not Nepomuk) */
    void removePerson(const QUrl &uri);
    /** Adds contacts to existing PIMO:Person */
    void addContactsToPerson(const QUrl &personUri, const QList<QUrl> &contacts);
    /** Removes given contacts from existing PIMO:Person */
    void removeContactsFromPerson(const QUrl &personUri, const QList<QUrl> &contacts);

    friend class ResourceWatcherService;
    friend class ContactItem;

    PersonsModelPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(PersonsModel);
};
}


#endif // PERSONS_MODEL_H
