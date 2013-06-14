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


#include "impersonsdatasource.h"
#include "personsmodel.h"

#include <TelepathyQt/AccountManager>
#include <TelepathyQt/AccountFactory>
#include <TelepathyQt/ContactManager>
#include <TelepathyQt/PendingOperation>
#include <TelepathyQt/PendingReady>

#include <KTp/contact-factory.h>
#include <KTp/global-contact-manager.h>
#include <KTp/types.h>

#include <TelepathyQt/Presence>

#include <KDebug>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Nepomuk2/ResourceManager>

class IMPersonsDataSource::Private {
public:
    KTp::GlobalContactManager *contactManager;
    Tp::AccountManagerPtr accountManager;
    QHash<QString, KTp::ContactPtr> contacts;
    IMPersonsDataSource *q;

    void onAllKnownContactsChanged(const Tp::Contacts &contactsAdded, const Tp::Contacts &contactsRemoved);
};

void IMPersonsDataSource::Private::onAllKnownContactsChanged(const Tp::Contacts &contactsAdded, const Tp::Contacts &contactsRemoved)
{
    if (!contacts.isEmpty()) {
        Q_FOREACH (const Tp::ContactPtr &contact, contactsRemoved) {
            contacts.remove(contact->id());
        }
    }

    Q_FOREACH (const Tp::ContactPtr &contact, contactsAdded) {
        KTp::ContactPtr ktpContact = KTp::ContactPtr::qObjectCast(contact);
        contacts.insert(contact->id(), ktpContact);

        Q_EMIT q->contactChanged(contact->id());

        connect(ktpContact.data(), SIGNAL(presenceChanged(Tp::Presence)),
                q, SLOT(onContactChanged()));

        connect(ktpContact.data(), SIGNAL(capabilitiesChanged(Tp::ContactCapabilities)),
                q, SLOT(onContactChanged()));

        connect(ktpContact.data(), SIGNAL(invalidated()),
                q, SLOT(onContactInvalidated()));

        //TODO: add other stuff here etc

    }
}

//-----------------------------------------------------------------------------

IMPersonsDataSource::IMPersonsDataSource(QObject *parent, const QVariantList &data)
    : BasePersonsDataSource(parent)
    , d(new Private)
{
    d->q = this;
    Tp::registerTypes();
    Tp::AccountFactoryPtr  accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                       Tp::Features() << Tp::Account::FeatureCore
                                                                       << Tp::Account::FeatureCapabilities
                                                                       << Tp::Account::FeatureProtocolInfo
                                                                       << Tp::Account::FeatureProfile);

    Tp::ConnectionFactoryPtr connectionFactory = Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                               Tp::Features() << Tp::Connection::FeatureCore
                                                                               << Tp::Connection::FeatureRoster
                                                                               << Tp::Connection::FeatureRosterGroups
                                                                               << Tp::Connection::FeatureSelfContact);

    Tp::ContactFactoryPtr contactFactory = KTp::ContactFactory::create(Tp::Features()  << Tp::Contact::FeatureAlias
    << Tp::Contact::FeatureSimplePresence
    << Tp::Contact::FeatureCapabilities
    << Tp::Contact::FeatureClientTypes);

    Tp::ChannelFactoryPtr channelFactory = Tp::ChannelFactory::create(QDBusConnection::sessionBus());

    d->accountManager = Tp::AccountManager::create(QDBusConnection::sessionBus(),
                                                   accountFactory,
                                                   connectionFactory,
                                                   channelFactory,
                                                   contactFactory);

    connect(d->accountManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)),
            this, SLOT(onAccountManagerReady(Tp::PendingOperation*)));
}

IMPersonsDataSource::~IMPersonsDataSource()
{
    delete d;
}

QVariant IMPersonsDataSource::dataForContact(const QString &contactId, int role) const
{
    KTp::ContactPtr contact = d->contacts.value(contactId);

    //we need to handle only few roles here, all the rest must go to the source model
    switch (role) {
        case PersonsModel::PresenceTypeRole:
            if (!contact.isNull()) {
                return contact->presence().status();
            } else if (!contactId.isEmpty()) {
                return QLatin1String("offline");
            } else if (contactId.isEmpty()) {
                return QLatin1String("unknown");
            }
        break;
        case PersonsModel::PresenceDisplayRole:
            if (!contact.isNull()) {
                return contact->presence().displayString();
            } else if (!contactId.isEmpty()) {
                return KTp::Presence(Tp::Presence::offline()).displayString();
            } else if (contactId.isEmpty()) {
                return QVariant();
            }
        break;
        case PersonsModel::PresenceDecorationRole:
            if (!contact.isNull()) {
                return contact->presence().icon();
            } else if (!contactId.isEmpty()) {
                return KTp::Presence(Tp::Presence::offline()).icon();
            } else if (contactId.isEmpty()) {
                return QVariant();
            }
        break;
    }

    return QVariant();
}

void IMPersonsDataSource::onAccountManagerReady(Tp::PendingOperation *op)
{
    if (op->isError()) {
        kWarning() << "Failed to initialize AccountManager:" << op->errorName();
        kWarning() << op->errorMessage();

        return;
    }

    kDebug() << "Account manager ready";

    d->contactManager = new KTp::GlobalContactManager(d->accountManager, this);
    connect(d->contactManager, SIGNAL(allKnownContactsChanged(Tp::Contacts,Tp::Contacts)),
            this, SLOT(onAllKnownContactsChanged(Tp::Contacts,Tp::Contacts)));

    d->onAllKnownContactsChanged(d->contactManager->allKnownContacts(), Tp::Contacts());
}

void IMPersonsDataSource::onContactChanged()
{
    QString id = qobject_cast<Tp::Contact*>(sender())->id();

    Q_EMIT contactChanged(id);
}

void IMPersonsDataSource::onContactInvalidated()
{
    QString id = qobject_cast<Tp::Contact*>(sender())->id();

    d->contacts.remove(id);

    Q_EMIT contactChanged(id);
}

KTp::ContactPtr IMPersonsDataSource::contactForContactId(const QString &contactId) const
{
    return d->contacts.value(contactId);
}

Tp::AccountPtr IMPersonsDataSource::accountForContact(const KTp::ContactPtr& contact) const
{
    return d->contactManager->accountForContact(contact);
}

Tp::AccountManagerPtr IMPersonsDataSource::accountManager() const
{
    return d->accountManager;
}


#include "impersonsdatasource.moc"
