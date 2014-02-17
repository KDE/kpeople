/*
 * Copyright 2013  David Edmundson <davidedmundson@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "global.h"

#include "personmanager_p.h"
#include "personpluginmanager_p.h"
#include "abstractpersonaction.h"

#include <KIconLoader>
#include <QIcon>

static const KCatalogLoader i18nLoader("libkpeople");

//these namespace members expose the useful bits of PersonManager
//global.h should be included from every exported header file so namespace members are always visible

QString KPeople::mergeContacts(const QStringList &ids)
{
    return PersonManager::instance()->mergeContacts(ids);
}

bool KPeople::unmergeContact(const QString &id)
{
    return PersonManager::instance()->unmergeContact(id);
}

QList<QAction*> KPeople::actionsForPerson(const KABC::Addressee &person,
                                          const KABC::AddresseeList &contacts,
                                          QObject *parent)
{
    QList<QAction*> actions;
    Q_FOREACH(KPeople::AbstractPersonAction *plugin, PersonPluginManager::actions()) {
        actions << plugin->actionsForPerson(person, contacts, parent);
    }

    return actions;
}

QString KPeople::iconNameForPresenceString(const QString& presenceName)
{
    if (presenceName == QLatin1String("available")) {
        return "user-online";
    }

    if (presenceName == QLatin1String("away")) {
        return "user-away";
    }

    if (presenceName == QLatin1String("busy") || presenceName == QLatin1String("dnd")) {
        return "user-busy";
    }

    if (presenceName == QLatin1String("xa")) {
        return "user-away-extended";
    }

    if (presenceName == QLatin1String("hidden")) {
        return "user-invisible";
    }

    return "user-offline";
}

int KPeople::presenceSortPriority(const QString& presenceName)
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

    if (presenceName == QLatin1String("offline")) {
        return 6;
    }

    return 7;
}
