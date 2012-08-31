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
    d_ptr->personWatcher = new Nepomuk2::ResourceWatcher(this);
    d_ptr->personWatcher->addType(Nepomuk2::Vocabulary::PIMO::Person());

    connect(d_ptr->personWatcher, SIGNAL(resourceCreated(Nepomuk2::Resource,QList<QUrl>)),
            this, SIGNAL(personCreated(Nepomuk2::Resource,QList<QUrl>)));
    connect(d_ptr->personWatcher, SIGNAL(resourceRemoved(QUrl,QList<QUrl>)),
            this, SIGNAL(personRemoved(QUrl,QList<QUrl>)));
    connect(d_ptr->personWatcher, SIGNAL(propertyAdded(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)),
            this, SLOT(onPersonPropertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    connect(d_ptr->personWatcher, SIGNAL(propertyRemoved(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)),
            this, SLOT(onPersonPropertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    connect(d_ptr->personWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(onPersonPropertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    
    d_ptr->personWatcher->start();
    
    d_ptr->contactWatcher = new Nepomuk2::ResourceWatcher(this);
    d_ptr->contactWatcher->addType(Nepomuk2::Vocabulary::NCO::PersonContact());
    
    connect(d_ptr->contactWatcher, SIGNAL(resourceCreated(Nepomuk2::Resource,QList<QUrl>)),
            this, SIGNAL(contactCreated(Nepomuk2::Resource,QList<QUrl>)));
    connect(d_ptr->contactWatcher, SIGNAL(resourceRemoved(QUrl,QList<QUrl>)),
            this, SIGNAL(contactRemoved(QUrl,QList<QUrl>)));
    connect(d_ptr->contactWatcher, SIGNAL(propertyAdded(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)),
            this, SLOT(onContactPropertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    connect(d_ptr->contactWatcher, SIGNAL(propertyRemoved(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)),
            this, SLOT(onContactPropertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    connect(d_ptr->contactWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(onContactPropertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariant)));
    
    d_ptr->contactWatcher->start();
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
