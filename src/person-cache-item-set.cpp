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

#include "person-cache-item-set.h"
#include "person-cache-item-set_p.h"

#include "person-cache.h"

#include <KDebug>

/******************************** PersonCacheItemSet::Private *************************************/

PersonCacheItemSetPrivate::PersonCacheItemSetPrivate(PersonCache *pc)
  : personCache(pc),
    refCount(0)
{
    kDebug();
}

PersonCacheItemSetPrivate::~PersonCacheItemSetPrivate()
{
    kDebug();
}

void PersonCacheItemSetPrivate::ref()
{
    refCount.ref();
}

void PersonCacheItemSetPrivate::unref()
{
    refCount.deref();
}


/******************************** PersonCacheItemSet **********************************************/

PersonCacheItemSet::PersonCacheItemSet(PersonCacheItemSetPrivate *d)
  : QObject(0),
    d_ptr(d)
{
    kDebug();

    d_ptr->ref();
}

PersonCacheItemSet::~PersonCacheItemSet()
{
    d_ptr->unref();

    d_ptr->personCache->removeItemSet(this);
}

bool PersonCacheItemSet::operator==(const PersonCacheItemSet &other) const
{
    return (this->d_ptr == (&other)->d_ptr);
}

bool PersonCacheItemSet::operator!=(const PersonCacheItemSet &other) const
{
    return (this->d_ptr != (&other)->d_ptr);
}


#include "person-cache-item-set.moc"

