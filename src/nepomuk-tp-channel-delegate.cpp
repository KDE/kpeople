/*
    Basic Telepathy chat starter
    Copyright (C) 2011 Martin Klapetek <martin.klapetek@gmail.com>

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

#include "nepomuk-tp-channel-delegate.h"

#include <TelepathyQt/constants.h>
#include <TelepathyQt/PendingChannelRequest>
#include <TelepathyQt/PendingReady>
#include <TelepathyQt/ContactManager>
#include <TelepathyQt/PendingContacts>
#include <KDebug>

#define PREFERRED_TEXTCHAT_HANDLER QLatin1String("org.freedesktop.Telepathy.Client.KDE.TextUi")


class NepomukTpChannelDelegatePrivate {
public:
    Tp::AccountManagerPtr   accountManager;
    Tp::AccountPtr          account;
};

NepomukTpChannelDelegate::NepomukTpChannelDelegate(QObject* parent)
    : QObject(parent),
      d_ptr(new NepomukTpChannelDelegatePrivate)
{
    Q_D(NepomukTpChannelDelegate);
    Tp::registerTypes();

    // Start setting up the Telepathy AccountManager.
    Tp::AccountFactoryPtr  accountFactory = Tp::AccountFactory::create(QDBusConnection::sessionBus(),
                                                                       Tp::Features() << Tp::Account::FeatureCore
                                                                       << Tp::Account::FeatureAvatar
                                                                       << Tp::Account::FeatureCapabilities
                                                                       << Tp::Account::FeatureProtocolInfo
                                                                       << Tp::Account::FeatureProfile);

    Tp::ConnectionFactoryPtr connectionFactory = Tp::ConnectionFactory::create(QDBusConnection::sessionBus(),
                                                                               Tp::Features() << Tp::Connection::FeatureCore
                                                                               << Tp::Connection::FeatureSelfContact);

    Tp::ContactFactoryPtr contactFactory = Tp::ContactFactory::create(Tp::Features()  << Tp::Contact::FeatureAlias
    << Tp::Contact::FeatureAvatarData
    << Tp::Contact::FeatureSimplePresence
    << Tp::Contact::FeatureCapabilities);

    Tp::ChannelFactoryPtr channelFactory = Tp::ChannelFactory::create(QDBusConnection::sessionBus());

    d->accountManager = Tp::AccountManager::create(QDBusConnection::sessionBus(),
                                                  accountFactory,
                                                  connectionFactory,
                                                  channelFactory,
                                                  contactFactory);

    connect(d->accountManager->becomeReady(), SIGNAL(finished(Tp::PendingOperation*)),
            this, SLOT(onAccountManagerReady(Tp::PendingOperation*)));
}

NepomukTpChannelDelegate::~NepomukTpChannelDelegate()
{
    delete d_ptr;
}

void NepomukTpChannelDelegate::onAccountManagerReady(Tp::PendingOperation*)
{
    kDebug() << "Account manager ready, bitch";
}

void NepomukTpChannelDelegate::startChat(const QString& accountId, const QString& contactId)
{
    Q_D(NepomukTpChannelDelegate);
    if (!d->accountManager->isReady()) {
        kDebug() << "Account manager not ready, come back later";
        return;
    }
    d->account = d->accountManager->accountForPath(accountId);
    kDebug() << d->account.isNull();

    Tp::PendingContacts *contacts = d->account->connection()->contactManager()->contactsForIdentifiers(QStringList() << contactId);

    connect(contacts, SIGNAL(finished(Tp::PendingOperation*)),
            this, SLOT(finished(Tp::PendingOperation*)));
}

void NepomukTpChannelDelegate::finished(Tp::PendingOperation *op)
{
    Q_D(NepomukTpChannelDelegate);
    Tp::ChannelRequestHints hints;
    hints.setHint(QLatin1String("org.kde.telepathy"), QLatin1String("forceRaiseWindow"), QVariant(true));

    d->account->ensureTextChat(qobject_cast<Tp::PendingContacts*>(op)->contacts().first(),
                                                                        QDateTime::currentDateTime(),
                                                                        PREFERRED_TEXTCHAT_HANDLER,
                                                                        hints);
}
