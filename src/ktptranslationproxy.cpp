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
#include <KIconLoader>
#include <QPixmapCache>

using namespace KPeople;


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
            return mapToSource(proxyIndex).data(PersonsModel::PresenceIconNameRole);
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
        case KTp::ContactAvatarPixmapRole:
            return contactPixmap(proxyIndex);
        case KTp::IdRole:
            return mapToSource(proxyIndex).data(PersonsModel::IMsRole);
        case KTp::HeaderTotalUsersRole:
            return sourceModel()->rowCount(mapToSource(proxyIndex));
        case KTp::ContactGroupsRole:
            return mapToSource(proxyIndex).data(PersonsModel::GroupsRole);
    }

    QVariantList ret;

    int j = sourceModel()->rowCount(mapToSource(proxyIndex));
    bool isChildContact = false;
    if (j == 0) {
        isChildContact = true;
        j = 1;
    }

    for (int i = 0; i < j; i++) {
        const QString contactId = isChildContact ? mapToSource(proxyIndex).data(PersonsModel::IMsRole).toString()
                                                : mapToSource(proxyIndex).data(PersonsModel::IMsRole).toList().at(i).toString();
        const KTp::ContactPtr contact = imPlugin->contactForContactId(contactId);

        if (!contact.isNull()) {
            switch (role) {
                case KTp::ContactRole:
                    ret += QVariant::fromValue<KTp::ContactPtr>(contact);
                    break;
                case KTp::AccountRole:
                    ret += QVariant::fromValue<Tp::AccountPtr>(imPlugin->accountForContact(contact));
                    break;
                case KTp::ContactPresenceMessageRole:
                    ret += contact->presence().statusMessage();
                    break;
                case KTp::ContactIsBlockedRole:
                    ret += contact->isBlocked();
                    break;
                case KTp::ContactCanTextChatRole:
                    ret += true;
                    break;
                case KTp::ContactCanAudioCallRole:
                    ret += contact->audioCallCapability();
                    break;
                case KTp::ContactCanVideoCallRole:
                    ret += contact->videoCallCapability();
                    break;
                case KTp::ContactCanFileTransferRole:
                    ret += contact->fileTransferCapability();
                    break;
                case KTp::ContactClientTypesRole:
                    ret += contact->clientTypes();
                    break;
            }
        } else if (contact.isNull() && role == KTp::AccountRole) {
            //if the KTp contact is null, we still need the Tp account for that contact
            //so we can either group it properly or bring that account online if user
            //starts a chat with a contact that belongs to offline account
            ret += QVariant::fromValue<Tp::AccountPtr>(imPlugin->accountForContactId(contactId));
        }
    }

    return ret;
}

QVariant KTpTranslationProxy::translatePresence(const QVariant &presenceName) const
{
    if (presenceName == QLatin1String("available")) {
        return Tp::ConnectionPresenceTypeAvailable;
    }

    if (presenceName == QLatin1String("away")) {
        return Tp::ConnectionPresenceTypeAway;
    }

    if (presenceName == QLatin1String("busy") || presenceName == QLatin1String("dnd")) {
        return Tp::ConnectionPresenceTypeBusy;
    }

    if (presenceName == QLatin1String("xa")) {
        return Tp::ConnectionPresenceTypeExtendedAway;
    }

    if (presenceName == QLatin1String("hidden")) {
        return Tp::ConnectionPresenceTypeHidden;
    }

    return Tp::ConnectionPresenceTypeOffline;
}

QPixmap KTpTranslationProxy::contactPixmap(const QModelIndex &index) const
{
    QPixmap avatar;

    int presenceType = index.data(KTp::ContactPresenceTypeRole).toInt();
    const QVariantList ids = index.data(KTp::IdRole).toList();
    QString id;
    if (!ids.isEmpty()) {
        id = ids.first().toString();
    }

    const QString keyCache = id + (presenceType == Tp::ConnectionPresenceTypeOffline ? QLatin1String("-offline") : QLatin1String("-online"));

    //check pixmap cache for the avatar, if not present, load the avatar
    if (!QPixmapCache::find(keyCache, avatar)){
        const QVariantList files = index.data(KTp::ContactAvatarPathRole).toList();
        QString file;
        if (!files.isEmpty()) {
            file = files.first().toUrl().toLocalFile();
        }

        //QPixmap::load() checks for empty path
        avatar.load(file);

        //if the above didn't succeed, we need to load the generic icon
        if (avatar.isNull()) {
            avatar = KIconLoader::global()->loadIcon(QLatin1String("im-user"), KIconLoader::NoGroup, 96);
        }

        //if the contact is offline, gray it out
        if (presenceType == Tp::ConnectionPresenceTypeOffline) {
            QImage image = avatar.toImage();
            const QPixmap alpha = avatar.alphaChannel();
            for (int i = 0; i < image.width(); ++i) {
                for (int j = 0; j < image.height(); ++j) {
                    int colour = qGray(image.pixel(i, j));
                    image.setPixel(i, j, qRgb(colour, colour, colour));
                }
            }
            avatar = avatar.fromImage(image);
            avatar.setAlphaChannel(alpha);
        }

        //insert the contact into pixmap cache for faster lookup
        QPixmapCache::insert(keyCache, avatar);
    }

    return avatar;
}
