/*
 * This file is part of kpeople
 *
 * Copyright (C) 2011 Collabora Ltd. <info@collabora.co.uk>
 *   @author George Goldberg <george.goldberg@collabora.co.uk>
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

#ifndef LIBKPEOPLE_PERSON_CACHE_ITEM_SET_P_H
#define LIBKPEOPLE_PERSON_CACHE_ITEM_SET_P_H

#include <QtCore/QAtomicInt>
#include <QtCore/QtGlobal>
#include <QHash>
#include <QUrl>

class PersonCacheItem;
class BasicPersonCacheItemFacet;
class PersonCache;
class PersonCacheItemSet;

class PersonCacheItemSetPrivate {

    Q_DISABLE_COPY(PersonCacheItemSetPrivate);

public:
    virtual ~PersonCacheItemSetPrivate();

    void ref();
    void unref();

    QHash<QUrl, PersonCacheItem*> data();

    PersonCache *personCache;

private:
    explicit PersonCacheItemSetPrivate(QHash<QUrl, PersonCacheItem*> personSet, PersonCache *personCache);

    friend class PersonCache;

    QAtomicInt refCount;
    QHash<QUrl, PersonCacheItem*> m_data;
};


#endif  // Header guard

