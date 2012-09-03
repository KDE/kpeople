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
#include "persons-model-item.h"

#include <Nepomuk2/Resource>
#include <nepomuk2/resourcewatcher.h>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <KDebug>

struct ResourceWatcherServicePrivate {
    ResourceWatcherServicePrivate(PersonsModel* model)
        : contactWatcher(0), m_model(model)
    { }

    Nepomuk2::ResourceWatcher *personWatcher;
    Nepomuk2::ResourceWatcher *contactWatcher;
    PersonsModel* m_model;

};

ResourceWatcherService::ResourceWatcherService(PersonsModel *parent)
    : QObject(parent),
      d_ptr(new ResourceWatcherServicePrivate(parent))
{
    ResourceWatcherServicePrivate* d = d_ptr;
    d->personWatcher = new Nepomuk2::ResourceWatcher(this);
    d->personWatcher->addType(Nepomuk2::Vocabulary::PIMO::Person());

    connect(d->personWatcher, SIGNAL(resourceCreated(Nepomuk2::Resource,QList<QUrl>)),
            this, SLOT(personCreated(Nepomuk2::Resource,QList<QUrl>)));
    connect(d->personWatcher, SIGNAL(resourceRemoved(QUrl,QList<QUrl>)),
            this, SLOT(personRemoved(QUrl,QList<QUrl>)));
    connect(d->personWatcher, SIGNAL(propertyAdded(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)),
            this, SLOT(onPersonPropertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    connect(d->personWatcher, SIGNAL(propertyRemoved(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)),
            this, SLOT(onPersonPropertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
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
            this, SLOT(onContactPropertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    connect(d->contactWatcher, SIGNAL(propertyRemoved(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)),
            this, SLOT(onContactPropertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    connect(d->contactWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(onContactPropertyModified(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)));
    
    d->contactWatcher->start();
}

ResourceWatcherService::~ResourceWatcherService()
{}

void ResourceWatcherService::onPersonPropertyChanged(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property, const QVariant& value)
{
    kDebug() << "stuff2";
    //check if the occurrences changed
}

void ResourceWatcherService::onPersonPropertyModified(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property,
                                                      const QVariantList& removed, const QVariantList& after)
{
    kDebug() << "stuff3";
    //check if the occurrences changed
}

void ResourceWatcherService::onContactPropertyChanged(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property, const QVariant& value)
{
    kDebug() << "stuff";
    //just modify addData()
}

void ResourceWatcherService::onContactPropertyModified(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property,
                                                      const QVariantList& removed, const QVariantList& after)
{
    kDebug() << "stuff4";
    //modify
}

void ResourceWatcherService::contactCreated(const Nepomuk2::Resource& res, const QList< QUrl >& types)
{
    //check if it has a person
    //add it or otherwise we shouldn't do anything
}

void ResourceWatcherService::personCreated(const Nepomuk2::Resource& res, const QList< QUrl >& types)
{
    Q_D(ResourceWatcherService);
    Q_ASSERT(!d->m_model->indexForUri(res.uri()).isValid());
    
    //check for its contacts
    //if it has no contacts it shouldn't be added until the groundingOccurrences change
}

void ResourceWatcherService::contactRemoved(const QUrl& uri, const QList< QUrl >&)
{
    Q_D(ResourceWatcherService);
    QModelIndex idx = d->m_model->indexForUri(uri);
    if(uri.isValid())
        d->m_model->removeRow(idx.row());
}

void ResourceWatcherService::personRemoved(const QUrl& uri, const QList< QUrl >&)
{
    Q_D(ResourceWatcherService);
    QModelIndex idx = d->m_model->indexForUri(uri);
    if(uri.isValid())
        d->m_model->removeRow(idx.row());
}
