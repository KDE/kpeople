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

#include <QStandardItemModel>

class PersonsModelFeature;
// class PersonsPresenceModel;
class ContactItem;
class KJob;
class QUrl;
struct PersonsModelPrivate;

namespace Nepomuk2 { class Resource; }
namespace Soprano { namespace Util { class AsyncQuery; } }

class KPEOPLE_EXPORT PersonsModel : public QStandardItemModel
{
    Q_OBJECT
    Q_DISABLE_COPY(PersonsModel)

public:
    enum Feature {
        FeatureNone       = 0x0000,
        FeatureIM         = 0x0001,
        FeatureGroups     = 0x0002,
        FeatureAvatars    = 0x0004,
        FeatureEmails     = 0x0008,
        FeatureFullName   = 0x0010,
        FeatureAll = FeatureIM |
                     FeatureGroups |
                     FeatureAvatars |
                     FeatureEmails |
                     FeatureFullName
    };
    Q_DECLARE_FLAGS(Features, Feature)
    Q_FLAGS(Features)

    enum Role {
        ContactIdRole = Qt::UserRole, ///uniqueID of contact STRING
        UriRole, //nepomuk URI STRING
        ChildContactUriRole, //since the model will be almost always used with proxies, we need to have a way
                             //to access child contact uri from fake persons; index.child() won't always work
                             //as the proxy might hide the child nodes, so we need another way to get its uri
        FullNamesRole, //nco:fullname //STRINGLIST
        EmailsRole, //nco:email //STRINGLIST
        NicknamesRole, //nco:   //STRINGLIST
        PhonesRole, //nco:phones //STRINGLIST
        IMsRole, //STRINGLIST
        IMAccountTypeRole, //FIXME: needed by IM feature; investigate if really needed

        PhotosRole,

        //move these to presence model
        PresenceTypeRole, //ENUM (NEW)
        PresenceDisplayRole, //  most online presence
        PresenceDecorationRole,

        GroupsRole, //groups STRINGLIST

        UserRole ///< in case it's needed to extend, use this one to start from
    };

    /**
     * @p mandatoryFeatures features we want to be sure the contact has
     * @p optionalFeatures optional contact features we'd like to have
     */
    PersonsModel(Features mandatoryFeatures = 0, Features optionalFeatures = 0, QObject *parent = 0);

    virtual ~PersonsModel();

    /**
     * Sets the features we want to construct a query for; this starts populating the model
     * @p mandatoryFeatures features we want to be sure the contact has
     * @p optionalFeatures optional contact features we'd like to have
     */
    Q_SCRIPTABLE void setQueryFlags(PersonsModel::Features mandatoryFeatures, PersonsModel::Features optionalFeatures);

    /**
     * @return QPair of query flags used to populate the model, first is mandatory, second is optional
     */
    QPair<PersonsModel::Features, PersonsModel::Features> queryFlags() const;

    /** Creates a pimo:person with contacts as groundingOccurances */
    void createPersonFromContacts(const QList<QUrl> &contacts);

    /**
     * Creates a pimo:person from indexes, checking if one of them isn't person already and
     * adding the contacts to it if it is
     */
    Q_SCRIPTABLE void createPersonFromIndexes(const QList<QModelIndex> &indexes);

    /** Adds contacts to existing PIMO:Person */
    void addContactsToPerson(const QUrl &personUri, const QList<QUrl> &contacts);

    /** Removes given contacts from existing PIMO:Person */
    void removeContactsFromPerson(const QUrl &personUri, const QList<QUrl> &contacts);

    /** Removes the link between all contacts, removes the pimo:person but leaves the contacts intact */
    void removePerson(const QUrl &uri);
    //FIXME: maybe merge with ^ ?
    void removePersonFromModel(const QModelIndex &index);

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
//     void findDuplicatesFinished(KJob *finder);

    void updateContactNextReady(Soprano::Util::AsyncQuery *query);
    void updateContactFinished(Soprano::Util::AsyncQuery *query);
    void updateContact(ContactItem *contact);

//     void duplicatesFound(QHash<QString, QSet<QPersistentModelIndex> > duplicates);

private:
    /**
     * @return actual features used to populate the model
     */
    QList<PersonsModelFeature> modelFeatures() const;

    void addPerson(const Nepomuk2::Resource &res);
    void addContact(const Nepomuk2::Resource &res);
    ContactItem* contactForIMAccount(const QUrl &uri) const;

    QModelIndex findRecursively(int role, const QVariant &value, const QModelIndex &idx = QModelIndex()) const;
    QList<PersonsModelFeature> init(PersonsModel::Features mandatoryFeatures, PersonsModel::Features optionalFeatures);

    friend class ResourceWatcherService;
    friend class ContactItem;

    PersonsModelPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(PersonsModel);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PersonsModel::Features)

#endif // PERSONS_MODEL_H
