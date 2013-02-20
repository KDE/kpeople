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


#include "ktp-translation-proxy.h"
#include "persons-presence-model.h"
#include "persons-model.h"

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
    switch (role) {
        case KTp::ContactPresenceTypeRole:
            if (proxyIndex.data(PersonsModel::StatusRole).toString() == "available") {
                return Tp::ConnectionPresenceTypeAvailable;
            } else if (proxyIndex.data(PersonsModel::StatusRole).toString() == "away") {
                return Tp::ConnectionPresenceTypeAway;
            } else if (proxyIndex.data(PersonsModel::StatusRole).toString() == "busy" ||
                proxyIndex.data(PersonsModel::StatusRole).toString() == "dnd") {

                return Tp::ConnectionPresenceTypeBusy;
            } else if (proxyIndex.data(PersonsModel::StatusRole).toString() == "xa") {
                return Tp::ConnectionPresenceTypeExtendedAway;
            } else {
                return Tp::ConnectionPresenceTypeOffline;
            }
            break;

        case Qt::DisplayRole:
            //this is needed to avoid infinite recursion call (proxyIndex.data(Qt::DisplayRole) would call this method)
            return mapToSource(proxyIndex).data(Qt::DisplayRole);
        case KTp::RowTypeRole:
            if (proxyIndex.data(PersonsModel::ResourceTypeRole) == PersonsModel::Contact) {
                return KTp::ContactRowType;
            } else {
                return KTp::PersonRowType;
            }
        case KTp::ContactAvatarPathRole:
            return proxyIndex.data(PersonsModel::PhotoRole);
        case KTp::ContactGroupsRole:
            return proxyIndex.data(PersonsModel::ContactGroupsRole);
        case KTp::AccountRole:
            return proxyIndex.data(PersonsModel::IMAccountRole);
        case KTp::IdRole:
            return proxyIndex.data(PersonsModel::IMRole);
        case KTp::ContactRole:
            return proxyIndex.data(PersonsModel::IMContactRole);
        case KTp::ContactIsBlockedRole:
            return proxyIndex.data(PersonsModel::BlockedRole);
    }

    return QIdentityProxyModel::data(proxyIndex, role);
}
