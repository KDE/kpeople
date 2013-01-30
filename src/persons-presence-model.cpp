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
    if (!m_presences.isEmpty()) {
        Q_FOREACH (const Tp::ContactPtr &contact, contactsRemoved) {
            m_presences.remove(contact->id());
        }
    }

    Q_FOREACH (const Tp::ContactPtr &contact, contactsAdded) {
        m_presences.insert(contact->id(), contact);

        connect(contact.data(), SIGNAL(presenceChanged(Tp::Presence)),
                this, SLOT(onContactChanged()));

        connect(contact.data(), SIGNAL(capabilitiesChanged(Tp::ContactCapabilities)),
                this, SLOT(onContactChanged()));

        //TODO: add other stuff here etc

        QModelIndex index = qobject_cast<PersonsModel*>(sourceModel())->findRecursively(PersonsModel::IMRole, contact->id());
        Q_EMIT dataChanged(index, index);
    }
}

void PersonsPresenceModel::onContactChanged()
{
    QString id = qobject_cast<Tp::Contact*>(sender())->id();

    QModelIndex index = qobject_cast<PersonsModel*>(sourceModel())->findRecursively(PersonsModel::IMRole, id);
    Q_EMIT dataChanged(index, index);
}

QVariant PersonsPresenceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == PersonsModel::StatusRole) {
        if (index.data(PersonsModel::ResourceTypeRole).toUInt() == PersonsModel::Contact) {
            QString contactId = index.data(PersonsModel::IMRole).toString();
            if (m_presences.keys().contains(contactId)) {
                return m_presences.value(contactId)->presence().status();
            } else if (!contactId.isEmpty()) {
                return QLatin1String("offline");
            } else if (contactId.isEmpty()) {
                return QLatin1String("unknown");
            }
        } else if (index.data(PersonsModel::ResourceTypeRole).toUInt() == PersonsModel::Person) {
            return queryChildrenForData(index, role);
        }
            }
            return ret;
        }
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
