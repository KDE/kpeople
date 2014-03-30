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
#include <Akonadi/Item>

namespace KPeople {
    class PersonDataPrivate {
    public:
        QStringList contactIds;
        MetaContact metaContact;
        QList<ContactMonitorPtr> watchers;
        KABC::Addressee customContact;
    };
}

using namespace KPeople;

KPeople::PersonData::PersonData(const QString &id, QObject* parent):
    QObject(parent),
    d_ptr(new PersonDataPrivate)
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
        //load the correct data source for this contact ID
        const QString sourceId = contactId.left(contactId.indexOf("://"));
        BasePersonsDataSource *dataSource = PersonPluginManager::dataSource(sourceId);
        if (dataSource) {
            ContactMonitorPtr cw = dataSource->contactMonitor(contactId);
            d->watchers << cw;

            //if the data source already has the contact set it already
            //if not it will be loaded when the contactChanged signal is emitted
            if (!cw->contact().isEmpty()) {
                contacts[contactId] = cw->contact();

                if (cw->contact().custom("kpeople", "customContact") == "1") {
                    d->customContact = cw->contact();
                }
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

KJob* PersonData::saveCustomContact(const KABC::Addressee &customContact)
{
    Akonadi::AgentInstance customContactResource = KPeople::customContactsResource();

    if (!customContactResource.isValid()) {
        // Ideally we should return a job that fails immediately
        // (hint in Tp::PendingFailure)
        return 0;
    }

    Akonadi::CollectionFetchJob *job = new Akonadi::CollectionFetchJob(Akonadi::Collection::root(),
                                                                       Akonadi::CollectionFetchJob::FirstLevel,
                                                                       this);

    job->exec();

    if (job->error()) {
        kWarning() << "Fetching collections failed;" << job->errorText();
        return 0;
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

    return createJob;
}
