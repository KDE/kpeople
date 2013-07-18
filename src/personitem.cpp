/*
    Persons Model Item
    Represents one person in the model
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>

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


#include "personitem.h"
#include "contactitem.h"

#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Soprano/Vocabulary/NAO>

using namespace KPeople;

PersonItem::PersonItem(const QUrl &personUri)
{
    setData(personUri, PersonsModel::UriRole);
}

QUrl PersonItem::uri() const
{
    return QStandardItem::data(PersonsModel::UriRole).toUrl();
}


QVariant PersonItem::queryChildrenForRole(int role) const
{
    QVariant ret;
    for (int i = 0; i < rowCount(); i++) {
        QVariant value = child(i)->data(role);
        if (!value.isNull()) {
            ret = value;
            break;
        }
    }
    return ret;
}

QVariantList PersonItem::queryChildrenForRoleList(int role) const
{
    QVariantList ret;
    for (int i = 0; i < rowCount(); i++) {
        QVariant value = child(i)->data(role);
        if (value.type() == QVariant::List) {
            ret += value.toList();
        } else {
            ret += value;
        }
    }
    return ret;
}

QVariant PersonItem::data(int role) const
{
    if (role == PersonsModel::PresenceTypeRole
        || role == PersonsModel::PresenceDisplayRole
        || role == PersonsModel::PresenceDecorationRole
        || role == PersonsModel::PresenceIconNameRole) {

        QVariantList presences = queryChildrenForRoleList(PersonsModel::PresenceTypeRole);

        if (presences.isEmpty()) {
            return QVariant();
        }

        //we find which position in the list contains the most online presence
        //and then we use that index to return the other roles
        int mostOnlineIndex = -1;
        int mostOnlinePresence = 999;

        for (int i = 0; i < presences.size(); i++) {
            int currentPresencePriority = presenceSortPriority(presences.at(i).toString());
            if (currentPresencePriority < mostOnlinePresence) {
                mostOnlineIndex = i;
                mostOnlinePresence = currentPresencePriority;

                //if the most online is "available",
                //break as there can't be anything more online
                if (mostOnlinePresence == 0) {
                    break;
                }
            }
        }

        Q_ASSERT(mostOnlineIndex != -1);

        switch (role) {
            case PersonsModel::PresenceTypeRole:
                return presences.at(mostOnlineIndex);
            case PersonsModel::PresenceDisplayRole: {
                const QVariantList presenceDisplayNames = queryChildrenForRoleList(PersonsModel::PresenceDisplayRole);
                return presenceDisplayNames.at(mostOnlineIndex);
            }
            case PersonsModel::PresenceDecorationRole: {
                const QVariantList presenceDecoration = queryChildrenForRoleList(PersonsModel::PresenceDecorationRole);
                return presenceDecoration.at(mostOnlineIndex);
            }
            case PersonsModel::PresenceIconNameRole: {
                const QVariantList presenceIconNames = queryChildrenForRoleList(PersonsModel::PresenceIconNameRole);
                return presenceIconNames.at(mostOnlineIndex);
            }
        }
    }

    if (role == PersonsModel::UriRole) {
        //return child contact uri instead of fake uri for fake persons
        if (QStandardItem::data(PersonsModel::UriRole).toString().startsWith("fakeperson")) {
            return child(0)->data(PersonsModel::UriRole);
        }
        return QStandardItem::data(role);
    }

    QVariantList ret;
    for (int i = 0; i < rowCount(); i++) {
        QVariant value;
        //if we're being asked for the child contacts uris,
        //we need to query the children for their UriRole
        if (role == PersonsModel::ChildContactsUriRole) {
            value = child(i)->data(PersonsModel::UriRole);
        } else {
            value = child(i)->data(role);
        }
        //these roles must return single QVariant
        if (role == Qt::DisplayRole || role == Qt::DecorationRole) {
            return value;
        }
        if (value.type() == QVariant::List) {
            ret += value.toList();
        } else if (!value.isNull()) {
            ret += value;
        }
    }

    if (ret.isEmpty()) {
        //we need to return empty qvariant here, otherwise we'd get a qvariant
        //with empty qvariantlist, which would get parsed as non-empty qvariant
        return QVariant();
    }

    return ret;
}

int PersonItem::presenceSortPriority(const QString &presenceName) const
{
    if (presenceName == QLatin1String("available")) {
        return 0;
    }

    if (presenceName == QLatin1String("busy") || presenceName == QLatin1String("dnd")) {
        return 1;
    }

    if (presenceName == QLatin1String("hidden")) {
        return 2;
    }

    if (presenceName == QLatin1String("away")) {
        return 3;
    }

    if (presenceName == QLatin1String("xa")) {
        return 4;
    }

    if (presenceName == QLatin1String("unknown")) {
        return 5;
    }

    return 6;
}

void PersonItem::contactDataChanged()
{
    emitDataChanged();
}
