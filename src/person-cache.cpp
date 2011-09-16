/*
 * This file is part of kpeople
 *
 * Copyright (C) 2010-2011 Collabora Ltd. <info@collabora.co.uk>
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

#include "person-cache.h"

#include <KDebug>
#include <KGlobal>


/******************************** PersonCache::Private ********************************************/

class PersonCachePrivate {

protected:
    Q_DECLARE_PUBLIC(PersonCache);
    PersonCache * const q_ptr;

public:
    explicit PersonCachePrivate(PersonCache *q)
    : q_ptr(q)
    { }

    virtual ~PersonCachePrivate()
    { }
};


/****************************** Global Static Stuff ***********************************************/

class PersonCacheHelper {

public:
    PersonCacheHelper()
      : q(0)
    { }

    ~PersonCacheHelper()
    {
        delete q;
    }

    PersonCache *q;
};

K_GLOBAL_STATIC(PersonCacheHelper, s_globalPersonCache)


/******************************** PersonCache *****************************************************/


PersonCache *PersonCache::instance()
{
    if (!s_globalPersonCache->q) {
        new PersonCache;
    }

    return s_globalPersonCache->q;
}

PersonCache::PersonCache()
  : QObject(0),
    d_ptr(new PersonCachePrivate(this))
{
    kDebug();

    Q_ASSERT(!s_globalPersonCache->q);
    s_globalPersonCache->q = this;
}

PersonCache::~PersonCache()
{
    kDebug();

    delete d_ptr;
}


#include "person-cache.moc"

