/*
    Watches for contact changes from the PersonsDataSource
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

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

#ifndef DATA_SOURCE_WATCHER_H
#define DATA_SOURCE_WATCHER_H

#include <QObject>
#include <QUrl>

#include "kpeople_export.h"

struct DataSourceWatcherPrivate;

class KPEOPLE_EXPORT DataSourceWatcher : public QObject
{
    Q_OBJECT
public:
    DataSourceWatcher(QObject *parent);

    virtual ~DataSourceWatcher();

    /** Add a watch for a contact
     * @arg the id of the contact to watch i.e foo@example.com
     * @arg the URI of this contact with the specified ID, to be emitted whenever the contactID changes
     */
    void watchContact(const QString &contactId, const QUrl &contactUri);

    void clearWatchedContacts();

Q_SIGNALS:
    /** Emitted whenever a contact changes state
        @arg contactUri the URI of the contact that changed
     */
    void contactChanged(const QUrl &contactUri);

private Q_SLOTS:
    void onContactChanged(const QString &contactId);

private:
    Q_DECLARE_PRIVATE(DataSourceWatcher)
    DataSourceWatcherPrivate * d_ptr;
};

#endif

