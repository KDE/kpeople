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


#include "resource-watcher-service.h"
#include "persons-model.h"
#include "person-item.h"
#include "contact-item.h"

#include <Nepomuk2/Resource>
#include <nepomuk2/resourcewatcher.h>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Variant>
#include <KDebug>

struct ResourceWatcherServicePrivate {
    ResourceWatcherServicePrivate(PersonsModel *model)
        : contactWatcher(0), m_model(model)
    { }

    Nepomuk2::ResourceWatcher *personWatcher;
    Nepomuk2::ResourceWatcher *contactWatcher;
    Nepomuk2::ResourceWatcher *imWatcher;
    PersonsModel *m_model;

};

ResourceWatcherService::ResourceWatcherService(PersonsModel *parent)
    : QObject(parent),
      d_ptr(new ResourceWatcherServicePrivate(parent))
{
    ResourceWatcherServicePrivate *d = d_ptr;
    d->personWatcher = new Nepomuk2::ResourceWatcher(this);
    d->personWatcher->addType(Nepomuk2::Vocabulary::PIMO::Person());

    connect(d->personWatcher, SIGNAL(resourceCreated(Nepomuk2::Resource,QList<QUrl>)),
            this, SLOT(personCreated(Nepomuk2::Resource,QList<QUrl>)));
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
            this, SLOT(contactCreated(Nepomuk2::Resource,QList<QUrl>)));
    connect(d->contactWatcher, SIGNAL(resourceRemoved(QUrl,QList<QUrl>)),
            this, SLOT(contactRemoved(QUrl,QList<QUrl>)));
    connect(d->contactWatcher, SIGNAL(propertyAdded(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)),
            this, SLOT(onContactPropertyAdded(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    connect(d->contactWatcher, SIGNAL(propertyRemoved(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)),
            this, SLOT(onContactPropertyRemoved(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    connect(d->contactWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(onContactPropertyModified(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)));

    d->contactWatcher->start();

    d->imWatcher = new Nepomuk2::ResourceWatcher(this);
    d->imWatcher->addType(Nepomuk2::Vocabulary::NCO::IMAccount());

    connect(d->imWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(onIMAccountPropertyModified(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)));

    d->imWatcher->start();
}

ResourceWatcherService::~ResourceWatcherService()
{}

void ResourceWatcherService::onPersonPropertyAdded(const Nepomuk2::Resource &res, const Nepomuk2::Types::Property &property, const QVariant &value)
{
    kDebug() << "person property added:" /*<< res.uri() */<< property.name() << value;

    if (property.uri() == Nepomuk2::Vocabulary::PIMO::groundingOccurrence()) {
        Q_D(ResourceWatcherService);
        PersonItem *item = static_cast<PersonItem*>(d->m_model->itemFromIndex(d->m_model->indexForUri(res.uri())));
        if (item) {
            item->addContacts(QList<QUrl>() << value.toUrl());
        } else {
            kWarning() << "Non-existing person specified!";
        }
    }
}

void ResourceWatcherService::onPersonPropertyRemoved(const Nepomuk2::Resource &res, const Nepomuk2::Types::Property &property, const QVariant &_value)
{
    kDebug() << "person property removed:" /*<< res.uri() */<< property.name() << _value;

    if (property.name() == "groundingOccurrence") {
        Q_D(ResourceWatcherService);
        PersonItem *item = static_cast<PersonItem*>(d->m_model->itemFromIndex(d->m_model->indexForUri(res.uri())));
        if (item) {
            if (_value.canConvert<QUrl>()) {    //one contact was removed from groundingOccurrence
                item->removeContacts(QList<QUrl>() << _value.toUrl());
            } else if (_value.canConvert<QList<QUrl> >()) {
                item->removeContacts(_value.value<QList<QUrl> >());
            }
        }
    }
}

template <typename T>
QList<T> qvariantlist_cast(const QVariantList &vals)
{
    QList<T> ret;
    foreach (const QVariant &v, vals)
        ret += v.value<T>();
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
    Q_D(ResourceWatcherService);
    if (property == Nepomuk2::Vocabulary::PIMO::groundingOccurrence()) {
        PersonItem *item = static_cast<PersonItem*>(d->m_model->itemFromIndex(d->m_model->indexForUri(res.uri())));
        if (item) {
            item->removeContacts(qvariantlist_cast<QUrl>(removed));
            item->addContacts(qvariantlist_cast<QUrl>(added));
        }
    }
}

void ResourceWatcherService::onContactPropertyAdded(const Nepomuk2::Resource &res, const Nepomuk2::Types::Property &property, const QVariant &value)
{
    kDebug() << "contact property added:" /*<< res.uri() */<< property.name() << value;

    Q_D(ResourceWatcherService);
    ContactItem *item = static_cast<ContactItem*>(d->m_model->itemFromIndex(d->m_model->indexForUri(res.uri())));
    if (item) {
        if (property.uri() == Nepomuk2::Vocabulary::NCO::photo()) {
            Nepomuk2::Resource photoRes(value.toUrl());
            item->addData(Nepomuk2::Vocabulary::NIE::url(), photoRes.property(Nepomuk2::Vocabulary::NIE::url()).toUrl());
        } else if (property.uri() == Nepomuk2::Vocabulary::NCO::hasIMAccount()) {
            item->pullResourceProperties(Nepomuk2::Resource(value.toUrl()));
        } else {
            item->addData(property.uri(), value);
        }
    }
}

void ResourceWatcherService::onContactPropertyRemoved(const Nepomuk2::Resource &res, const Nepomuk2::Types::Property &property, const QVariant &value)
{
    kDebug() << "contact property removed:" /*<< res.uri() */<< property.name() << value;

    Q_D(ResourceWatcherService);
    ContactItem *item = static_cast<ContactItem*>(d->m_model->itemFromIndex(d->m_model->indexForUri(res.uri())));
    if (item) {
        item->removeData(property.uri());
    }
}

void ResourceWatcherService::onContactPropertyModified(const Nepomuk2::Resource &res, const Nepomuk2::Types::Property &property,
                                                      const QVariantList &added, const QVariantList &removed)
{
    kDebug() << "contact changed:" << res.uri() << property.name() << removed << added;

    if (!removed.isEmpty() && !added.isEmpty()) {
        Q_D(ResourceWatcherService);
        ContactItem *item = static_cast<ContactItem*>(d->m_model->itemFromIndex(d->m_model->indexForUri(res.uri())));

        if (item) {
            item->modifyData(property.uri(), added);
        }
    }
}

void ResourceWatcherService::onIMAccountPropertyModified(const Nepomuk2::Resource &res, const Nepomuk2::Types::Property &property, const QVariantList &added, const QVariantList &removed)
{
    kDebug() << "IM contact changed:" << res.uri() << property.name() << removed << added;

    Q_D(ResourceWatcherService);
    kDebug() << d->m_model->contactForIMAccount(res.uri());
    ContactItem *item = static_cast<ContactItem*>(d->m_model->contactForIMAccount(res.uri()));
    if (!item) {
        return;
    }

    //we expect only the imNickname to change here
    if (property.name() == QLatin1String("imNickname")) {
        item->modifyData(property.uri(), added.first());
    }
}

void ResourceWatcherService::contactCreated(const Nepomuk2::Resource &res, const QList< QUrl > &types)
{
    kDebug() << "new contact" /*<< res.uri() */<< types;
    Q_D(ResourceWatcherService);

    QModelIndex idx = d->m_model->indexForUri(res.uri());
    if (!idx.isValid()) {
        d->m_model->createContact(res);
    }
}

void ResourceWatcherService::personCreated(const Nepomuk2::Resource &res, const QList< QUrl > &types)
{
    kDebug() << "new person" /*<< res.uri() */<< types;
    Q_D(ResourceWatcherService);
    Q_ASSERT(res.hasProperty(Nepomuk2::Vocabulary::PIMO::groundingOccurrence()));
    QModelIndex idx = d->m_model->indexForUri(res.uri());
    if (!idx.isValid()) {
        d->m_model->createPerson(res);
    }
}

void ResourceWatcherService::contactRemoved(const QUrl &uri, const QList< QUrl >&)
{
    Q_D(ResourceWatcherService);
    QModelIndex idx = d->m_model->indexForUri(uri);
    if (uri.isValid()) {
        d->m_model->removeRow(idx.row());
    }
    kDebug() << "contact removed" << uri;
}

void ResourceWatcherService::personRemoved(const QUrl &uri)
{
    Q_D(ResourceWatcherService);
    QModelIndex idx = d->m_model->indexForUri(uri);
    if (idx.isValid()) {
        d->m_model->removePersonFromModel(idx);
    }
    kDebug() << "person removed" << uri;
}
