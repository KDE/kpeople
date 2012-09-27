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


#ifndef PEOPLE_RESOURCE_WATCHER_SERVICE_H
#define PEOPLE_RESOURCE_WATCHER_SERVICE_H

#include <QObject>
#include <Nepomuk2/Resource>
#include <Nepomuk2/Types/Property>

class PersonsModel;
class ResourceWatcherServicePrivate;

class ResourceWatcherService : public QObject
{
    Q_OBJECT
public:
    ResourceWatcherService(PersonsModel* model);
    virtual ~ResourceWatcherService();

private Q_SLOTS:
    void onPersonPropertyAdded(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property, const QVariant& value);
    void onContactPropertyAdded(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property, const QVariant& value);
    void onPersonPropertyRemoved(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property, const QVariant& value);
    void onContactPropertyRemoved(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property, const QVariant& value);
    void onPersonPropertyModified(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property,
                                  const QVariantList& removed, const QVariantList& after);
    void onContactPropertyModified(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property,
                                  const QVariantList& removed, const QVariantList& after);
    void onIMAccountPropertyModified(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property,
                                     const QVariantList& added, const QVariantList& removed);
    void personCreated(const Nepomuk2::Resource& res, const QList<QUrl>& types);
    void personRemoved(const QUrl& uri);
    void contactCreated(const Nepomuk2::Resource& res, const QList<QUrl>& types);
    void contactRemoved(const QUrl& uri, const QList< QUrl >& types);
    
    void updateIMAccount(const Nepomuk2::Resource& res);

private:
    ResourceWatcherServicePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(ResourceWatcherService)
};

#endif // RESOURCE_WATCHER_SERVICE_H
