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

#include <KTp/Models/contacts-model.h>
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
        case ContactsModel::PresenceTypeRole:
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
        case ContactsModel::AliasRole:
            //this is needed to avoid infinite recursion call (proxyIndex.data(Qt::DisplayRole) would call this method)
            return mapToSource(proxyIndex).data(Qt::DisplayRole);
        case ContactsModel::TypeRole:
            if (proxyIndex.data(PersonsModel::ResourceTypeRole) == PersonsModel::Contact) {
                return ContactsModel::ContactRowType;
            } else {
                return ContactsModel::GroupRowType;
            }
        case ContactsModel::AvatarRole:
            return proxyIndex.data(PersonsModel::PhotoRole);
        case ContactsModel::GroupsRole:
            return proxyIndex.data(PersonsModel::ContactGroupsRole);
        case ContactsModel::AccountRole:
            return proxyIndex.data(PersonsModel::IMAccountRole);
        case ContactsModel::ContactRole:
            return proxyIndex.data(PersonsModel::IMContactRole);
    }

    return QIdentityProxyModel::data(proxyIndex, role);
}
