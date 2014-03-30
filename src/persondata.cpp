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

#include <Akonadi/CollectionFetchJob>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/ItemModifyJob>
#include <Akonadi/Item>

namespace KPeople {
    class PersonDataPrivate {
    public:
        PersonDataPrivate(PersonData *parent):
            q_ptr(parent) {}

        QString personId;
        QStringList contactIds;
        MetaContact metaContact;
        QList<ContactMonitorPtr> watchers;
        KABC::Addressee customContact;
        QString customContactId;
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
        d->personId = id;
    } else {
        d->contactIds = PersonManager::instance()->contactsForPersonId(personId);
    }

    KABC::Addressee::Map contacts;
    Q_FOREACH(const QString &contactId, d->contactIds) {
        ContactMonitorPtr contactMonitor = d->addContactMonitor(contactId);
        if (contactMonitor && !contactMonitor->contact().isEmpty()) {
            if (contactMonitor->contact().custom("kpeople", "customContact") == "1") {
                d->customContact = contactMonitor->contact();
                d->customContactId = contactId;
            }
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

KABC::Addressee PersonData::customContact() const
{
    Q_D(const PersonData);

    if (!d->customContact.isEmpty()) {
        return d->customContact;
    }

    return KABC::Addressee();
}

void PersonData::saveCustomContact(const KABC::Addressee &customContact)
{
    Q_D(PersonData);

    Akonadi::AgentInstance customContactResource = KPeople::customContactsResource();

    if (!customContactResource.isValid()) {
        // Ideally we should return a job that fails immediately
        // (hint in Tp::PendingFailure)
        return;
    }

    // If the custom contact already exists, we need to issue ItemModifyJob
    // If not, we'll continue with ItemCreateJob
    if (!d->customContact.isEmpty()) {
        Akonadi::Item customContactItem = Akonadi::Item::fromUrl(d->customContactId);
        customContactItem.setMimeType("text/directory");
        customContactItem.setPayload<KABC::Addressee>(customContact);

        Akonadi::ItemModifyJob *modifyJob = new Akonadi::ItemModifyJob(customContactItem, this);
        modifyJob->exec();

        if (modifyJob->error()) {
            kWarning() << "Failed to finish the custom contact modify job:" << modifyJob->errorText();
        }

        return;
    }

    Akonadi::CollectionFetchJob *job = new Akonadi::CollectionFetchJob(Akonadi::Collection::root(),
                                                                       Akonadi::CollectionFetchJob::FirstLevel,
                                                                       this);

    job->exec();

    if (job->error()) {
        kWarning() << "Fetching collections failed;" << job->errorText();
        return;
    }

    Akonadi::Collection customCollection;

    Q_FOREACH (const Akonadi::Collection &collection, job->collections()) {
        if (collection.resource() == customContactResource.identifier()) {
            customCollection = collection;
            break;
        }
    }

    job->deleteLater();

    KABC::Addressee nonConstCopy = customContact;
    nonConstCopy.insertCustom("kpeople","customContact", "1");

    Akonadi::Item customContactItem;
    customContactItem.setMimeType("text/directory");
    customContactItem.setPayload<KABC::Addressee>(nonConstCopy);

    Akonadi::ItemCreateJob *createJob = new Akonadi::ItemCreateJob(customContactItem, customCollection, this);
    createJob->exec();

    if (createJob->error()) {
        kWarning() << "Failed to store the custom contact:" << createJob->errorText();
        return;
    }

    KPeople::mergeContacts(QStringList() << d->personId << createJob->item().url().prettyUrl());
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
