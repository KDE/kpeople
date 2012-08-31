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
    Q_D(ResourceWatcherService);
    d->personWatcher = new Nepomuk2::ResourceWatcher(this);
    d->personWatcher->addType(Nepomuk2::Vocabulary::PIMO::Person());

    connect(d->personWatcher, SIGNAL(resourceCreated(Nepomuk2::Resource,QList<QUrl>)),
            this, SIGNAL(personCreated(Nepomuk2::Resource,QList<QUrl>)));
    connect(d->personWatcher, SIGNAL(resourceRemoved(QUrl,QList<QUrl>)),
            this, SIGNAL(personRemoved(QUrl,QList<QUrl>)));
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
            this, SIGNAL(contactCreated(Nepomuk2::Resource,QList<QUrl>)));
    connect(d->contactWatcher, SIGNAL(resourceRemoved(QUrl,QList<QUrl>)),
            this, SIGNAL(contactRemoved(QUrl,QList<QUrl>)));
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

void ResourceWatcherService::onContactPropertyChanged(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property, const QVariant& value)
{
    kDebug() << "stuff";
}

void ResourceWatcherService::onPersonPropertyChanged(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property, const QVariant& value)
{
    kDebug() << "stuff2";
}

void ResourceWatcherService::onPersonPropertyModified(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property,
                                                      const QVariantList& removed, const QVariantList& after)
{
    kDebug() << "stuff3";
}

void ResourceWatcherService::onContactPropertyModified(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property,
                                                      const QVariantList& removed, const QVariantList& after)
{
    kDebug() << "stuff4";
}
