/*
    Watches for contact changes from the PersonsDataSource
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

#include "datasourcewatcher.h"

#include "personpluginmanager.h"
#include "basepersonsdatasource.h"

using namespace KPeople;

struct DataSourceWatcherPrivate
{
    QHash<QString, QUrl> watchedContactsMap;
};

DataSourceWatcher::DataSourceWatcher(QObject *parent):
    QObject(parent),
    d_ptr(new DataSourceWatcherPrivate)
{
    connect(PersonPluginManager::presencePlugin(), SIGNAL(contactChanged(QString)), SLOT(onContactChanged(QString)));
}

DataSourceWatcher::~DataSourceWatcher()
{
    delete d_ptr;
}

void DataSourceWatcher::watchContact(const QString &contactId, const QUrl &contactUri)
{
    Q_D(DataSourceWatcher);
    d->watchedContactsMap[contactId] = contactUri;
}

void DataSourceWatcher::clearWatchedContacts()
{
    Q_D(DataSourceWatcher);
    d->watchedContactsMap.clear();
}

void DataSourceWatcher::onContactChanged(const QString &contactId)
{
    Q_D(DataSourceWatcher);

    //if we are watching this contact, emit that it has changed. Otherwise do nothing
    QHash<QString, QUrl>::const_iterator it = d->watchedContactsMap.constFind(contactId);
    if (it != d->watchedContactsMap.constEnd()) {
        Q_EMIT contactChanged(it.value());
    }
}


