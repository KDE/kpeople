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

#include "metacontact_p.h"
#include "personmanager_p.h"
#include "personpluginmanager_p.h"
#include "basepersonsdatasource.h"
#include "contactmonitor.h"

#include <QDebug>

namespace KPeople {
    class PersonDataPrivate {
    public:
        PersonDataPrivate(PersonData *parent):
            q_ptr(parent) {}

        QString personId;
        QStringList contactIds;
        MetaContact metaContact;
        QList<ContactMonitorPtr> watchers;

        ContactMonitorPtr addContactMonitor(const QString &contactId);
    private:
        Q_DECLARE_PUBLIC(PersonData);
        PersonData *q_ptr;
    };
}

using namespace KPeople;

KPeople::PersonData::PersonData(const QString &id, QObject* parent):
    QObject(parent),
    d_ptr(new PersonDataPrivate(this))
{
    Q_D(PersonData);

    QString personId;
    //query DB
    if (id.startsWith("kpeople://")) {
        personId = id;
    } else {
        personId = PersonManager::instance()->personIdForContact(id);
    }

    if (personId.isEmpty()) {
        d->contactIds = QStringList() << id;
    } else {
        d->contactIds = PersonManager::instance()->contactsForPersonId(personId);
    }

    KABC::Addressee::Map contacts;
    Q_FOREACH(const QString &contactId, d->contactIds) {
        ContactMonitorPtr contactMonitor = d->addContactMonitor(contactId);
        if (contactMonitor && !contactMonitor->contact().isEmpty()) {
            contacts[contactId] = contactMonitor->contact();
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
    if (d->metaContact.contactIds().contains(watcher->contactId())) {
        d->metaContact.updateContact(watcher->contactId(), watcher->contact());
    } else {
        d->metaContact.insertContact(watcher->contactId(), watcher->contact());
    }
    Q_EMIT dataChanged();
}

void PersonData::onContactAddedToPerson(const QString &personId, const QString &contactId)
{
    Q_D(PersonData);

    //we check both in case we have opened this up for
    //akonadi://123 which is not in a metacontact and then it becomes part of a metacontact
    //we would still want to merge
    if (d->personId == personId || d->personId == contactId) {
        d->contactIds << contactId;

        ContactMonitorPtr contactMonitor = d->addContactMonitor(contactId);

        //if contact is loaded already
        if (contactMonitor && !contactMonitor->contact().isEmpty()) {
            d->metaContact.insertContact(contactId, contactMonitor->contact());
        }
    }
}

void PersonData::onContactRemovedFromPerson(const QString &contactId)
{
    Q_D(PersonData);

    if (d->contactIds.contains(contactId)) {
        d->contactIds.removeOne(contactId);
    }
    d->metaContact.removeContact(contactId);
}

ContactMonitorPtr PersonDataPrivate::addContactMonitor(const QString &contactId)
{
    Q_Q(PersonData);

    //load the correct data source for this contact ID
    const QString sourceId = contactId.left(contactId.indexOf("://"));
    BasePersonsDataSource *dataSource = PersonPluginManager::dataSource(sourceId);
    if (dataSource) {
        ContactMonitorPtr cw = dataSource->contactMonitor(contactId);
        watchers << cw;
        q->connect(cw.data(), SIGNAL(contactChanged()), q, SLOT(onContactChanged()));
        return cw;
    }
    return ContactMonitorPtr();
}

