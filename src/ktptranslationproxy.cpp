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


#include "ktptranslationproxy.h"
#include "personsmodel.h"
#include "personpluginmanager.h"
#include "plugins/ktp-plugin/impersonsdatasource.h"

#include <KTp/types.h>
#include <KDebug>

KTpTranslationProxy::KTpTranslationProxy(QObject *parent)
    : QIdentityProxyModel(parent)
{
}

KTpTranslationProxy::~KTpTranslationProxy()
{
}

QVariant KTpTranslationProxy::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid()) {
        return QVariant();
    }

    IMPersonsDataSource *imPlugin = qobject_cast<IMPersonsDataSource*>(PersonPluginManager::presencePlugin());

    if (!imPlugin) {
        kWarning() << "No imPlugin";
        return QVariant();
    }

    switch (role) {
        case KTp::ContactPresenceTypeRole:
            return translatePresence(mapToSource(proxyIndex).data(PersonsModel::PresenceTypeRole));
        case KTp::ContactPresenceIconRole:
            return mapToSource(proxyIndex).data(PersonsModel::PresenceDecorationRole);
        case KTp::ContactPresenceNameRole:
            return mapToSource(proxyIndex).data(PersonsModel::PresenceDisplayRole);
        case Qt::DisplayRole:
            return mapToSource(proxyIndex).data(Qt::DisplayRole);
        case KTp::RowTypeRole:
            if (sourceModel()->rowCount(mapToSource(proxyIndex)) <= 1) {
                return KTp::ContactRowType;
            } else {
                return KTp::PersonRowType;
            }
        case KTp::ContactAvatarPathRole:
            return mapToSource(proxyIndex).data(PersonsModel::PhotosRole);
        case KTp::IdRole:
            return mapToSource(proxyIndex).data(PersonsModel::IMsRole);
        case KTp::HeaderTotalUsersRole:
            return sourceModel()->rowCount(mapToSource(proxyIndex));
        case KTp::ContactGroupsRole:
            return mapToSource(proxyIndex).data(PersonsModel::GroupsRole);
    }

    if (sourceModel()->rowCount(mapToSource(proxyIndex)) == 1) {
        const QString contactId = mapToSource(proxyIndex).data(PersonsModel::IMsRole).toList().first().toString();
        const KTp::ContactPtr contact = imPlugin->contactForContactId(contactId);

        if (!contact.isNull()) {
            switch (role) {
                case KTp::ContactRole:
                    return QVariant::fromValue<KTp::ContactPtr>(contact);
                case KTp::AccountRole:
                    return QVariant::fromValue<Tp::AccountPtr>(imPlugin->accountForContact(contact));
                case KTp::ContactPresenceMessageRole:
                    return contact->presence().statusMessage();
                case KTp::ContactIsBlockedRole:
                    return contact->isBlocked();
                case KTp::ContactCanTextChatRole:
                    return true;
                case KTp::ContactCanAudioCallRole:
                    return contact->audioCallCapability();
                case KTp::ContactCanVideoCallRole:
                    return contact->videoCallCapability();
                case KTp::ContactCanFileTransferRole:
                    return contact->fileTransferCapability();
                case KTp::ContactClientTypesRole:
                    return contact->clientTypes();
            }
        } else if (contact.isNull() && role == KTp::AccountRole) {
            //if the KTp contact is null, we still need the Tp account for that contact
            //so we can either group it properly or bring that account online if user
            //starts a chat with a contact that belongs to offline account
            return QVariant::fromValue<Tp::AccountPtr>(imPlugin->accountForContactId(contactId));
        }
    }

    return QIdentityProxyModel::data(proxyIndex, role);
}

QVariantList KTpTranslationProxy::translatePresence(const QVariant &presenceList) const
{
    QVariantList presences;
    Q_FOREACH (const QString &presenceName, presenceList.toStringList()) {
        if (presenceName == QLatin1String("available")) {
            presences << Tp::ConnectionPresenceTypeAvailable;
        }

        if (presenceName == QLatin1String("away")) {
            presences << Tp::ConnectionPresenceTypeAway;
        }

        if (presenceName == QLatin1String("busy") || presenceName == QLatin1String("dnd")) {
            presences << Tp::ConnectionPresenceTypeBusy;
        }

        if (presenceName == QLatin1String("xa")) {
            presences << Tp::ConnectionPresenceTypeExtendedAway;
        }

        if (presenceName == QLatin1String("hidden")) {
            presences << Tp::ConnectionPresenceTypeHidden;
        }

        presences << Tp::ConnectionPresenceTypeOffline;
    }

    return presences;
}
