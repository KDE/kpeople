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


/****************************** PersonCacheItem::Private ******************************************/

class PersonCacheItemPrivate {

protected:
    Q_DECLARE_PUBLIC(PersonCacheItem);
    PersonCacheItem * const q_ptr;

public:
    explicit PersonCacheItemPrivate(PersonCacheItem *q)
    : q_ptr(q)
    { }

    virtual ~PersonCacheItemPrivate()
    { }
};


/****************************** PersonCacheItem ***************************************************/

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

