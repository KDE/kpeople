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

    Q_FOREACH (const Tp::AccountPtr &account, m_accountManager->allAccounts()) {
        onNewAccountAdded(account);
    }

}

void PersonsPresenceModel::onNewAccountAdded(const Tp::AccountPtr &account)
{
    connect(account.data(), SIGNAL(connectionChanged(Tp::ConnectionPtr)),
            this, SLOT(onAccountConnectionChanged(Tp::ConnectionPtr)));

    onAccountConnectionChanged(account->connection());
}

void PersonsPresenceModel::onAccountConnectionChanged(const Tp::ConnectionPtr &connection)
{
    if (!connection.isNull()) {
        kDebug() << "Got" << connection->contactManager()->allKnownContacts().size() << "contacts, inserting presences...";
        Q_FOREACH (const Tp::ContactPtr &contact, connection->contactManager()->allKnownContacts()) {
            m_presences.insert(contact->id(), contact->presence());

            connect(contact.data(), SIGNAL(presenceChanged(Tp::Presence)),
                    this, SLOT(onContactPresenceChanged(Tp::Presence)));

            QModelIndex index = qobject_cast<PersonsModel*>(sourceModel())->findRecursively(PersonsModel::IMRole, contact->id());
            Q_EMIT dataChanged(index, index);
        }
    }


}

void PersonsPresenceModel::onContactPresenceChanged(const Tp::Presence &presence)
{
    QString id = qobject_cast<Tp::Contact*>(sender())->id();
    m_presences.insert(id, presence);

    QModelIndex index = qobject_cast<PersonsModel*>(sourceModel())->findRecursively(PersonsModel::IMRole, id);
    Q_EMIT dataChanged(index, index);
}

QVariant PersonsPresenceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == PersonsModel::StatusRole) {
        QString contactId = index.data(PersonsModel::IMRole).toString();

        if (m_presences.keys().contains(contactId)) {
            return m_presences.value(contactId).status();
        } else {
            return QLatin1String("unknown");
        }
    }

    return QIdentityProxyModel::data(index, role);
}
