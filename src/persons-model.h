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
class PersonsPresenceModel;
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
        FeatureContactUID = 0x0020,
        FeatureBirthday   = 0x0040,
        FeatureAll = FeatureIM |
                     FeatureGroups |
                     FeatureAvatars |
                     FeatureEmails |
                     FeatureFullName
    };
    Q_DECLARE_FLAGS(Features, Feature)

    enum Role {
        ContactTypeRole = Qt::UserRole,
        ContactIdRole,
        ContactsCountRole,
        UriRole,
        NameRole,
        LabelRole,
        EmailRole,
        NickRole,
        PhoneRole,
        IMRole,
        PhotoRole,
        IMAccountUriRole,
        IMAccountRole,
        IMAccountTypeRole,
        IMContactRole,
        BlockedRole,
        StatusRole,
        StatusStringRole,
        ResourceTypeRole,
        ContactGroupsRole,
        ContactCanTextChatRole,
        ContactCanAudioCallRole,
        ContactCanVideoCallRole,
        ContactCanFileTransferRole,
        ContactClientTypesRole,
        LastRole ///< in case it's needed to extend, use this one to start from
    };

    enum ResourceType {
        Person,
        Contact
    };

    /**
     * @p mandatoryFeatures features we want to be sure the contact has
     * @p optionalFeatures optional contact features we'd like to have
     */
    PersonsModel(PersonsModel::Features mandatoryFeatures = 0, PersonsModel::Features optionalFeatures = 0, QObject *parent = 0);

    /**
     * Sets the features we want to construct a query for; this starts populating the model
     * @p mandatoryFeatures features we want to be sure the contact has
     * @p optionalFeatures optional contact features we'd like to have
     */
    void setQueryFlags(PersonsModel::Features mandatoryFeatures, PersonsModel::Features optionalFeatures);

    /**
     * @return QPair of query flags used to populate the model, first is mandatory, second is optional
     */
    QPair<PersonsModel::Features, PersonsModel::Features> queryFlags() const;

    /**
     * @return actual features used to populate the model
     */
    QList<PersonsModelFeature> modelFeatures() const;

    /** Creates a pimo:person with contacts as groundingOccurances */
    void createPersonFromContacts(const QList<QUrl> &contacts);

    /**
     * Creates a pimo:person from indexes, checking if one of them isn't person already and
     * adding the contacts to it if it is
     */
    void createPersonFromIndexes(const QList<QModelIndex> &indexes);

    /** Adds contacts to existing PIMO:Person */
    void addContactsToPerson(const QUrl &personUri, const QList<QUrl> &contacts);

    /** Removes given contacts from existing PIMO:Person */
    void removeContactsFromPerson(const QUrl &personUri, const QList<QUrl> &contacts);

    /** Removes the link between all contacts, removes the pimo:person but leaves the contacts intact */
    void removePerson(const QUrl &uri);
    //FIXME: maybe merge with ^ ?
    void removePersonFromModel(const QModelIndex &index);

    QModelIndex indexForUri(const QUrl &uri) const;

    void findDuplicates();

    void createPerson(const Nepomuk2::Resource &res);
    void createContact(const Nepomuk2::Resource &res);
    ContactItem* contactForIMAccount(const QUrl &uri) const;

private Q_SLOTS:
    void jobFinished(KJob *job);
    void query(const QString &queryString);
    void nextReady(Soprano::Util::AsyncQuery *query);
    void queryFinished(Soprano::Util::AsyncQuery *query);
    void findDuplicatesFinished(KJob *finder);

Q_SIGNALS:
    void peopleAdded();
    void duplicatesFound(QHash<QString, QSet<QPersistentModelIndex> > duplicates);

private:
    QModelIndex findRecursively(int role, const QVariant &value, const QModelIndex &idx = QModelIndex()) const;
    QList<PersonsModelFeature> init(PersonsModel::Features mandatoryFeatures, PersonsModel::Features optionalFeatures);

    friend class PersonsPresenceModel;

    PersonsModelPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(PersonsModel);

};

Q_DECLARE_OPERATORS_FOR_FLAGS(PersonsModel::Features)

#endif // PERSONS_MODEL_H
