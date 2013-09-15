/*
    Resource watcher
    Copyright (C) 2011 Martin Klapetek <martin.klapetek@gmail.com>

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


#include "resourcewatcherservice_p.h"
#include "personsmodel.h"
#include "personitem_p.h"
#include "contactitem_p.h"
#include "personsmodelfeature.h"

#include <Nepomuk2/Resource>
#include <nepomuk2/resourcewatcher.h>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Variant>
#include <KDebug>


namespace KPeople {
struct ResourceWatcherServicePrivate {
    ResourceWatcherServicePrivate(PersonsModel *model)
        : contactWatcher(0), personsModel(model)
    { }

    Nepomuk2::ResourceWatcher *personWatcher;
    Nepomuk2::ResourceWatcher *contactWatcher;
    PersonsModel *personsModel;
};
}

using namespace KPeople;

ResourceWatcherService::ResourceWatcherService(PersonsModel *parent)
    : QObject(parent),
      d_ptr(new ResourceWatcherServicePrivate(parent))
{
    ResourceWatcherServicePrivate *d = d_ptr;
    d->personWatcher = new Nepomuk2::ResourceWatcher(this);
    d->personWatcher->addType(Nepomuk2::Vocabulary::PIMO::Person());

    connect(d->personWatcher, SIGNAL(resourceRemoved(QUrl,QList<QUrl>)),
            this, SLOT(personRemoved(QUrl)));
    connect(d->personWatcher, SIGNAL(propertyAdded(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)),
            this, SLOT(onPersonPropertyAdded(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    connect(d->personWatcher, SIGNAL(propertyRemoved(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)),
            this, SLOT(onPersonPropertyRemoved(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    connect(d->personWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(onPersonPropertyModified(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)));

    d->personWatcher->start();

    d->contactWatcher = new Nepomuk2::ResourceWatcher(this);
    d->contactWatcher->addType(Nepomuk2::Vocabulary::NCO::PersonContact());

    connect(d->contactWatcher, SIGNAL(resourceCreated(Nepomuk2::Resource,QList<QUrl>)),
            this, SLOT(contactCreated(Nepomuk2::Resource)));
    connect(d->contactWatcher, SIGNAL(resourceRemoved(QUrl,QList<QUrl>)),
            this, SLOT(contactRemoved(QUrl)));
    connect(d->contactWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(onContactPropertyModified(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)));

    d->contactWatcher->start();
}

ResourceWatcherService::~ResourceWatcherService()
{
    delete d_ptr;
}

void ResourceWatcherService::onPersonPropertyAdded(const Nepomuk2::Resource &res, const Nepomuk2::Types::Property &property, const QVariant &value)
{
    kDebug() << "person property added:" /*<< res.uri() */<< property.name() << value;

    if (property.uri() == Nepomuk2::Vocabulary::PIMO::groundingOccurrence()) {
        Q_D(ResourceWatcherService);
        PersonItem *item = d->personsModel->personItemForUri(res.uri());
        if (!item) {
            //if the person does not exist yet, create it first
            d->personsModel->addPerson(res.uri());
        }

        d->personsModel->addContactsToPerson(res.uri(), QList<QUrl>() << value.toUrl());
    }
}

void ResourceWatcherService::onPersonPropertyRemoved(const Nepomuk2::Resource &res, const Nepomuk2::Types::Property &property, const QVariant &value)
{
    kDebug() << "person property removed:" /*<< res.uri() */<< property.name() << value;

    if (property.uri() == Nepomuk2::Vocabulary::PIMO::groundingOccurrence()) {
        Q_D(ResourceWatcherService);
        PersonItem *item = d->personsModel->personItemForUri(res.uri());
        if (!item) {
            kWarning() << "Person does not exist in the model!";
            return;
        }

        d->personsModel->removeContactsFromPerson(res.uri(), QList<QUrl>() << value.toUrl());
    }
}

template <typename T>
QList<T> qvariantlist_cast(const QVariantList &vals)
{
    QList<T> ret;
    Q_FOREACH (const QVariant &v, vals) {
        ret += v.value<T>();
    }
    return ret;
}

void ResourceWatcherService::onPersonPropertyModified(const Nepomuk2::Resource &res, const Nepomuk2::Types::Property &property,
                                                      const QVariantList &added, const QVariantList &removed)
{
    kDebug() << "person changed:" /*<< res.uri() */<< property.name() << removed << added << " // " << res.property(property.uri());
    if (removed.isEmpty() || added.isEmpty()) {
        //if the removed or added is empty, it means the property was either added or removed, not really changed
        //in which case it is handled by the propertyAdded and/or propertyRemoved slots
        return;
    }
    //TODO: fill up once we have person editing available
}


void ResourceWatcherService::onContactPropertyModified(const Nepomuk2::Resource &res, const Nepomuk2::Types::Property &property,
                                                       const QVariantList &added, const QVariantList &removed)
{
    Q_UNUSED(added);
    Q_UNUSED(removed);
    kDebug() << "contact changed:" << property.name() << res.uri();

    Q_D(ResourceWatcherService);
    d->personsModel->updateContact(res.uri());
}

void ResourceWatcherService::contactCreated(const Nepomuk2::Resource &res)
{
    kDebug() << "new contact created:" << res.uri();

    Q_D(ResourceWatcherService);
    //addContact() will also load all wanted properties
    d->personsModel->addContact(res.uri());
}

void ResourceWatcherService::contactRemoved(const QUrl &uri)
{
    Q_D(ResourceWatcherService);
    kDebug() << "Removing contact with" << uri;
    d->personsModel->removeContact(uri);
}

void ResourceWatcherService::personRemoved(const QUrl &uri)
{
    Q_D(ResourceWatcherService);
    kDebug() << "Removing person with" << uri;
    d->personsModel->removePerson(uri);
}
