/*
 * This file is part of kpeople
 *
 * Copyright (C) 2010-2011 Collabora Ltd. <info@collabora.co.uk>
 *   @author George Goldberg <george.goldberg@collabora.co.uk>
 *   @author Martin Klapetek <martin.klapetek@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef LIBKPEOPLE_PERSON_CACHE_H
#define LIBKPEOPLE_PERSON_CACHE_H

#include <QtCore/QObject>

#include <Nepomuk/Resource>
#include <Nepomuk/Types/Property>

#include "kpeople_export.h"

class PersonsModelContactItem;
class PersonsModelItem;
class QStandardItem;
class PersonsModel;
class PersonCacheItemSet;
class PersonCachePrivate;

class KPEOPLE_EXPORT PersonCache : public QObject {

    Q_OBJECT
    Q_DECLARE_PRIVATE(PersonCache);
    Q_DISABLE_COPY(PersonCache);

public:
    virtual ~PersonCache();
    static PersonCache *instance();

    void startQuery();

Q_SIGNALS:
    void personAddedToCache(PersonsModelItem *person);
    void contactsFetched(const QHash<QUrl, PersonsModelItem *>& personNodes,
                         const QHash<PersonsModelItem *, QList<PersonsModelContactItem *> >& contactNodes);

private Q_SLOTS:
    void onNewPersonCreated(Nepomuk::Resource res, QList<QUrl> types);
    void onPersonRemoved();
    void onPersonPropertyChanged(Nepomuk::Resource res, Nepomuk::Types::Property property, QVariant value);

private:
    PersonCache();
    PersonCachePrivate * const d_ptr;
};

#endif  // Header guard
