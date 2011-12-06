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

#include "person-cache-item.h"

#include <KDebug>
#include <QUrl>

#include "person-cache-item_p.h"

PersonCacheItem::PersonCacheItem()
  : d_ptr(new PersonCacheItemPrivate(this))
{
    kDebug();
}

PersonCacheItem::~PersonCacheItem()
{
    kDebug();

    delete d_ptr;
}

void PersonCacheItem::addData(const QString& key, const QString& value)
{
    d_ptr->data.insert(key, value);
}

void PersonCacheItem::addHashData(const QString& key, const QUrl& uri)
{
    d_ptr->dataUri.insert(key, uri);
}

void PersonCacheItem::addFacet(PersonCacheItem::FacetTypes facets)
{
    d_ptr->facets |= facets;
}

void PersonCacheItem::setD(PersonCacheItemPrivate* d)
{
    if (d_ptr) {
        delete d_ptr;
    }

    d_ptr = d;
}

