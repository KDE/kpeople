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
#include <KUrl>

#include <QtCore/QHash>

#include <Soprano/Query/QueryLanguage>
#include <Soprano/QueryResultIterator>
#include <Soprano/Model>

#include <Nepomuk/ResourceManager>
#include <Nepomuk/Variant>

#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NCO>
#include <Soprano/Vocabulary/NAO>

#include "basic-person-cache-item-facet.h"
#include "person-cache-item.h"
#include "im-person-cache-item-facet.h"
#include "resource-watcher-service.h"

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
    QHash<QUrl, PersonCacheItem*> persons;

    ResourceWatcherService *watcher;

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

    d_ptr->watcher = new ResourceWatcherService(this);
    connect(d_ptr->watcher, SIGNAL(personCreated(Nepomuk::Resource,QList<QUrl>)),
            this, SLOT(onNewPersonCreated(Nepomuk::Resource,QList<QUrl>)));
}

PersonCache::~PersonCache()
{
    kDebug();

    delete d_ptr;
}

PersonCacheItemSet *PersonCache::query(const QString &query, PersonCacheItem::FacetTypes facetType, QList<QUrl> requestedKeys)
{
    kDebug();

    // TODO: Open the Nepomuk Context
    // TODO: Facet magic
    // TODO: All kinds of other shit

    // FIXME: Assume that the query is new

    Soprano::QueryResultIterator it = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery(query,
                                                                                                      Soprano::Query::QueryLanguageSparql);

    IMPersonCacheItemFacet *person;
    QHash<QUrl, PersonCacheItem*> set;

    QString keyString;

    while(it.next()) {
        QUrl currentUri = it[QLatin1String("uri")].uri();

        if (set.keys().contains(currentUri)) {
            person = dynamic_cast<IMPersonCacheItemFacet*>(set.value(currentUri));
        } else {
            person = new IMPersonCacheItemFacet(currentUri);
            set.insert(currentUri, person);
            person->addFacet(facetType);
        }

        Q_FOREACH(const QUrl &keyUri, requestedKeys) {
            keyString = keyUri.toString();
            //convert every key to correspond to the nepomuk bindings
            //FIXME: get this method out of the loop, it repeats too many times for nothing
            keyString = keyString.right(keyString.length() - keyString.lastIndexOf(QLatin1Char('/')) - 1).replace(QLatin1Char('#'), QLatin1Char('_'));

            person->addData(keyUri, it[keyString].toString());

            kDebug() << keyString << it[keyString].toString();
        }

    }

    d_ptr->persons.unite(set);

    // Create the PersonCacheItemSetPrivate instance.
    PersonCacheItemSetPrivate *pcisp = new PersonCacheItemSetPrivate(set, this);

    // Save it to the hash of query to PCISPs
    // FIXME: Use the actual query as the key
    d_ptr->queryResultSets.insert(query, pcisp);

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

void PersonCache::onNewPersonCreated(Nepomuk::Resource res, QList<QUrl> types)
{
    PersonCacheItem *person = new PersonCacheItem(res.uri());
    d_ptr->persons.insert(res.uri(), person);

    QHashIterator<QString, Nepomuk::Variant> i(res.allProperties());

    while (i.hasNext()) {
        i.next();
        kDebug() << i.key() << i.value();
    }

    emit personAddedToCache(person);
}

void PersonCache::onPersonRemoved()
{

}

void PersonCache::onPersonPropertyChanged(Nepomuk::Resource res, Nepomuk::Types::Property property, QVariant value)
{

}


#include "person-cache.moc"
