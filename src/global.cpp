/*
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

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
