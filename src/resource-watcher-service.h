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


#ifndef RESOURCE_WATCHER_SERVICE_H
#define RESOURCE_WATCHER_SERVICE_H

#include <QObject>
#include <Nepomuk/Resource>
#include <Nepomuk/Types/Property>

class ResourceWatcherServicePrivate;

class ResourceWatcherService : public QObject
{
    Q_OBJECT
public:
    ResourceWatcherService(QObject *parent = 0);
    virtual ~ResourceWatcherService();

Q_SIGNALS:
    void personCreated(Nepomuk::Resource, QList<QUrl>);
    void personRemoved(QUrl, QList<QUrl>);
    void propertyChanged();

public Q_SLOTS:
    void onPropertyChanged(Nepomuk::Resource res, Nepomuk::Types::Property property, QVariant value);

private:
    ResourceWatcherServicePrivate * const d_ptr;
};

#endif // RESOURCE_WATCHER_SERVICE_H
