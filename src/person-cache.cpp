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

#include "person-cache-item-set.h"
#include "person-cache-item-set_p.h"

#include <KDebug>
#include <KGlobal>

#include <QtCore/QHash>


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

    QHash<QString, PersonCacheItemSetPrivate*> queryResultSets;
    QMultiHash<PersonCacheItemSetPrivate*, PersonCacheItemSet*> itemSets;

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

PersonCacheItemSet *PersonCache::query()
{
    kDebug();

    // TODO: Open the Nepomuk Context
    // TODO: Facet magic
    // TODO: All kinds of other shit

    // FIXME: Assume that the query is new

    // Create the PersonCacheItemSetPrivate instance.
    PersonCacheItemSetPrivate *pcisp = new PersonCacheItemSetPrivate(this);

    // Save it to the hash of query to PCISPs
    // FIXME: Use the actual query as the key
    d_ptr->queryResultSets.insert(QString(), pcisp);

    // Create a PersonCacheItemSet to return.
    PersonCacheItemSet *pcis = new PersonCacheItemSet(pcisp);

    // Save the PCIS to the hash so that we can tell it when it should signal changes.
    d_ptr->itemSets.insert(pcisp, pcis);

    // FIXME: Connect up signals/slots so that when the nepomuk context signals something has
    //        happened we relay it to all the PCIS's.

    // Return the PCIS
    return pcis;
}

void PersonCache::removeItemSet(PersonCacheItemSet *itemSet)
{
    kDebug();

    // Remove the calling PCIS from the hash so we don't try to access it again.
    d_ptr->itemSets.remove(itemSet->d_ptr, itemSet);

    // Check if the ItemSetPrivate should be deleted.
    if (itemSet->d_ptr->refCount == 0) {
        // Delete it.
        // FIXME remove from d_ptr->queryResultSets.

        // FIXME: close the Nepomuk context
    }
}


#include "person-cache.moc"

