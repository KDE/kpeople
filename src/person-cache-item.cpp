/*
 * This file is part of kpeople
 *
 * Copyright (C) 2011 Collabora Ltd. <info@collabora.co.uk>
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

#include "person-cache-item.h"

#include <KDebug>
#include <QUrl>
#include <QStringList>

#include "person-cache-item_p.h"

PersonCacheItem::PersonCacheItem(const QUrl &uri)
  : d_ptr(new PersonCacheItemPrivate(this))
{
    d_ptr->uri = uri;
//     kDebug();
}

PersonCacheItem::~PersonCacheItem()
{
    kDebug();

    delete d_ptr;
}

void PersonCacheItem::addData(const QUrl &key, const QString &value)
{
    Q_D(PersonCacheItem);
    kDebug() << "Inserting" << value << "(" << key << ")";
    d->data.insert(key, value);
}

void PersonCacheItem::addData(const QUrl& key, const QStringList& values)
{
    Q_D(PersonCacheItem);
    Q_FOREACH (const QString &value, values) {
        kDebug() << "Inserting (multi)" << value << "(" << key << ")";
        d->data.insert(key, value);
    }
}

void PersonCacheItem::addHashData(const QString& key, const QUrl& uri)
{
    Q_D(PersonCacheItem);
    d->dataUri.insert(key, uri);
}

void PersonCacheItem::addFacet(PersonCacheItem::FacetTypes facets)
{
    Q_D(PersonCacheItem);
    d->facets |= facets;
}

QUrl PersonCacheItem::uri() const
{
    Q_D(const PersonCacheItem);
    return d->uri;
}

QString PersonCacheItem::data(const QUrl &key)
{
    Q_D(PersonCacheItem);
    return d->data.value(key);
}

QMultiHash<QUrl, QString> PersonCacheItem::dataHash() const
{
    Q_D(const PersonCacheItem);
    return d->data;
}

bool PersonCacheItem::hasFacet(PersonCacheItem::FacetType facet)
{
    Q_D(const PersonCacheItem);
    if (d->facets & facet) {
        return true;
    }

    return false;
}
