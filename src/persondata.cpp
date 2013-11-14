/*
    Copyright (C) 2013  David Edmundson (davidedmundson@kde.org)

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

#include "persondata.h"

#include "metacontact.h"
#include "personmanager.h"
#include "personpluginmanager.h"
#include "basepersonsdatasource.h"
#include "contactmonitor.h"

#include <QDebug>

namespace KPeople {
    class PersonDataPrivate {
    public:
        QStringList contactIds;
        MetaContact metaContact;
        QList<ContactMonitorPtr> watchers;
    };
}

using namespace KPeople;

KPeople::PersonData::PersonData(const QString &personId, QObject* parent):
    QObject(parent),
    d_ptr(new PersonDataPrivate)
{
    Q_D(PersonData);

    //query DB
    const QStringList contactIds;
    if (personId.startsWith("kpeople://")) {
        d->contactIds = PersonManager::instance()->contactsForPersonId(personId);
    } else {
        d->contactIds << personId;
    }
    KABC::Addressee::Map contacts;
    Q_FOREACH(BasePersonsDataSource *dataSource, PersonPluginManager::dataSourcePlugins()) {
        Q_FOREACH(const QString &contactId, d->contactIds) {
            //FIXME this is terrible.. we have to ask every datasource for the contact
            //future idea: plugins have a method of what their URIs will start with
            //then we keep plugins as a map
            ContactMonitorPtr cw = dataSource->contactMonitor(contactId);
            d->watchers << cw;
            if (!cw->contact().isEmpty()) {
                contacts[contactId] = cw->contact();
            }
            connect(cw.data(), SIGNAL(contactChanged()), SLOT(onContactChanged()));
        }
    }

    d->metaContact = MetaContact(personId, contacts);
}

PersonData::~PersonData()
{
    delete d_ptr;
}

KABC::Addressee PersonData::person() const
{
    Q_D(const PersonData);
    return d->metaContact.personAddressee();
}

KABC::AddresseeList PersonData::contacts() const
{
    Q_D(const PersonData);
    return d->metaContact.contacts();
}

void PersonData::onContactChanged()
{
    Q_D(PersonData);


    ContactMonitor *watcher = qobject_cast<ContactMonitor*>(sender());
    d->metaContact.updateContact(watcher->contactId(), watcher->contact());
    Q_EMIT dataChanged();
}
