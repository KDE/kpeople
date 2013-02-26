/*
    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>

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


#include "persons-presence-model.h"
#include "persons-model.h"

#include <TelepathyQt/AccountManager>
#include <TelepathyQt/AccountFactory>
#include <TelepathyQt/ContactManager>
#include <TelepathyQt/PendingOperation>
#include <TelepathyQt/PendingReady>

#include <KTp/contact-factory.h>
#include <KTp/global-contact-manager.h>
#include <KDebug>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Nepomuk2/ResourceManager>

PersonsPresenceModel::PersonsPresenceModel(QObject *parent)
: QIdentityProxyModel(parent)
{
    Tp::AccountFactoryPtr  accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                       Tp::Features() << Tp::Account::FeatureCore
                                                                       << Tp::Account::FeatureCapabilities
                                                                       << Tp::Account::FeatureProtocolInfo
                                                                       << Tp::Account::FeatureProfile);

    Tp::ConnectionFactoryPtr connectionFactory = Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                               Tp::Features() << Tp::Connection::FeatureCore
                                                                               << Tp::Connection::FeatureRoster
                                                                               << Tp::Connection::FeatureSelfContact);

    Tp::ContactFactoryPtr contactFactory = KTp::ContactFactory::create(Tp::Features()  << Tp::Contact::FeatureAlias
                                                                        << Tp::Contact::FeatureSimplePresence
                                                                        << Tp::Contact::FeatureCapabilities
                                                                        << Tp::Contact::FeatureClientTypes);

    Tp::ChannelFactoryPtr channelFactory = Tp::ChannelFactory::create(QDBusConnection::sessionBus());

    m_accountManager = Tp::AccountManager::create(QDBusConnection::sessionBus(),
                                                  accountFactory,
                                                  connectionFactory,
                                                  channelFactory,
                                                  contactFactory);

    connect(m_accountManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)),
            this, SLOT(onAccountManagerReady(Tp::PendingOperation*)));
}

PersonsPresenceModel::~PersonsPresenceModel()
{

}

void PersonsPresenceModel::onAccountManagerReady(Tp::PendingOperation *op)
{
    if (op->isError()) {
        kWarning() << "Failed to initialize AccountManager:" << op->errorName();
        kWarning() << op->errorMessage();

        return;
    }

    kDebug() << "Account manager ready";

    m_contactManager = new KTp::GlobalContactManager(m_accountManager, this);
    connect(m_contactManager, SIGNAL(allKnownContactsChanged(Tp::Contacts,Tp::Contacts)),
            this, SLOT(onAllKnownContactsChanged(Tp::Contacts,Tp::Contacts)));

    onAllKnownContactsChanged(m_contactManager->allKnownContacts(), Tp::Contacts());
}

void PersonsPresenceModel::onAllKnownContactsChanged(const Tp::Contacts &contactsAdded, const Tp::Contacts &contactsRemoved)
{
    if (!m_contacts.isEmpty()) {
        Q_FOREACH (const Tp::ContactPtr &contact, contactsRemoved) {
            m_contacts.remove(contact->id());
        }
    }

    Q_FOREACH (const Tp::ContactPtr &contact, contactsAdded) {
        KTp::ContactPtr ktpContact = KTp::ContactPtr::qObjectCast(contact);
        m_contacts.insert(contact->id(), ktpContact);

        connect(ktpContact.data(), SIGNAL(presenceChanged(Tp::Presence)),
                this, SLOT(onContactChanged()));

        connect(ktpContact.data(), SIGNAL(capabilitiesChanged(Tp::ContactCapabilities)),
                this, SLOT(onContactChanged()));

        connect(ktpContact.data(), SIGNAL(invalidated()),
                this, SLOT(onContactInvalidated()));

        //TODO: add other stuff here etc

    }

    reset();
}

void PersonsPresenceModel::onContactChanged()
{
    QString id = qobject_cast<Tp::Contact*>(sender())->id();

    QModelIndex index;
    if (sourceModel()) {
        index = qobject_cast<PersonsModel*>(sourceModel())->findRecursively(PersonsModel::IMRole, id);
    }
    Q_EMIT dataChanged(index, index);
}

void PersonsPresenceModel::onContactInvalidated()
{
    QString id = qobject_cast<Tp::Contact*>(sender())->id();

    m_contacts.remove(id);

    QModelIndex index;
    if (sourceModel()) {
        index = qobject_cast<PersonsModel*>(sourceModel())->findRecursively(PersonsModel::IMRole, id);
    }
    Q_EMIT dataChanged(index, index);
}

QVariant PersonsPresenceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    PersonsModel::ResourceType type;
    QString contactId;

    //we need to handle only few roles here, all the rest must go to the source model
    switch (role) {
        case PersonsModel::StatusRole:
        case PersonsModel::IMContactRole:
        case PersonsModel::IMAccountRole:
        case PersonsModel::BlockedRole:
        case PersonsModel::ContactCanTextChatRole:
        case PersonsModel::ContactCanAudioCallRole:
        case PersonsModel::ContactCanVideoCallRole:
        case PersonsModel::ContactCanFileTransferRole:

            type = (PersonsModel::ResourceType)mapToSource(index).data(PersonsModel::ResourceTypeRole).toUInt();
            contactId = mapToSource(index).data(PersonsModel::IMRole).toString();

            if (type == PersonsModel::Contact) {
                return dataForContactId(contactId, role);
            } else if (type == PersonsModel::Person) {
                return queryChildrenForData(index, role);
            }
        break;
    }

    return QIdentityProxyModel::data(index, role);
}

QVariantList PersonsPresenceModel::queryChildrenForData(const QModelIndex &index, int role) const
{
    QVariantList ret;
    for (int i = 0; i < rowCount(); i++) {
        QVariant value = index.child(i, 0).data(role);
        if (!value.isNull()) {
            ret += value;
        }
    }

    return ret;
}

QVariant PersonsPresenceModel::dataForContactId(const QString &contactId, int role) const
{
    KTp::ContactPtr contact = m_contacts.value(contactId);

    if (role == PersonsModel::StatusRole) {
        if (!contact.isNull()) {
            return contact->presence().status();
        } else if (!contactId.isEmpty()) {
            return QLatin1String("offline");
        } else if (contactId.isEmpty()) {
            return QLatin1String("unknown");
        }
    }

    if (role == PersonsModel::IMAccountRole) {
        if (!contact.isNull()) {
            return QVariant::fromValue<Tp::AccountPtr>(m_contactManager->accountForContact(m_contacts.value(contactId)));
        } else {
            QString accountId = queryNepomukForAccountId(contactId);
            //nepomuk stores account path, which is in form /org/freedesktop/Telepathy/Account/...
            //while GCM looks for uniqueIdentifier(), which is without this^ prefix, so we need to chop it off first
            return QVariant::fromValue<Tp::AccountPtr>(m_contactManager->accountForAccountId(accountId.remove(0,35)));
        }
    }

    if (contact.isNull()) {
        //no point doing any of those below...
        return QVariant();
    }

    if (role == PersonsModel::IMContactRole) {
        return QVariant::fromValue<KTp::ContactPtr>(contact);
    }

    if (role == PersonsModel::BlockedRole) {
        return contact->isBlocked();
    }

    if (role == PersonsModel::ContactCanTextChatRole) {
        return true; //FIXME: this is missing in KTp::ContactPtr
    }

    if (role == PersonsModel::ContactCanAudioCallRole) {
        return contact->audioCallCapability();
    }

    if (role == PersonsModel::ContactCanVideoCallRole) {
        return contact->videoCallCapability();
    }

    if (role == PersonsModel::ContactCanFileTransferRole) {
        return contact->fileTransferCapability();
    }

    return QVariant();
}

QString PersonsPresenceModel::queryNepomukForAccountId(const QString &contactId) const
{
    QString query = QString::fromUtf8(
        "select DISTINCT ?a "
        "WHERE { "
            "?x a nco:ContactMedium . "
            "?x nco:imID \"%1\"^^xsd:string . "
            "?x nco:isAccessedBy ?c . "
            "?c telepathy:accountIdentifier ?a . "
        "}").arg(contactId);

    Soprano::Model *model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery(query, Soprano::Query::QueryLanguageSparql);

    QString accountPath;

    while (it.next()) {
        accountPath = it[0].literal().toString();
    }

    return accountPath;
}
