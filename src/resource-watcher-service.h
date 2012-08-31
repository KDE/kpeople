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

Q_SIGNALS:
    void personCreated(const Nepomuk2::Resource& res, const QList<QUrl>&);
    void personRemoved(const QUrl& url, const QList<QUrl>&);
    void contactCreated(const Nepomuk2::Resource& res, const QList<QUrl>&);
    void contactRemoved(const QUrl& url, const QList<QUrl>&);

public Q_SLOTS:
    void onPersonPropertyChanged(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property, const QVariant& value);
    void onContactPropertyChanged(const Nepomuk2::Resource& res, const Nepomuk2::Types::Property& property, const QVariant& value);

private:
    ResourceWatcherServicePrivate * const d_ptr;
};

#endif // RESOURCE_WATCHER_SERVICE_H
