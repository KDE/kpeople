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

#include "person-cache.h"

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

#include "person-cache-item.h"
#include "resource-watcher-service.h"

#include "persons-model.h"

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

    QHash<QUrl, PersonCacheItem*> persons;
//     QMultiHash<PersonCacheItem::FacetType, QUrl> allRequestedKeys;

    ResourceWatcherService *watcher;

    PersonsModel *model;
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

//     d_ptr->watcher = new ResourceWatcherService(this);
//     connect(d_ptr->watcher, SIGNAL(personCreated(Nepomuk::Resource,QList<QUrl>)),
//             this, SLOT(onNewPersonCreated(Nepomuk::Resource,QList<QUrl>)));
}

PersonCache::~PersonCache()
{
    kDebug();

    delete d_ptr;
}

void PersonCache::query(const QString &query, QList<QUrl> requestedKeys)
{
    kDebug();

    Soprano::QueryResultIterator it = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery(query,
                                                                                                      Soprano::Query::QueryLanguageSparql);

    PersonCacheItem *person;
    QHash<QUrl, PersonCacheItem*> set;

    QString keyString;

    while(it.next()) {
        QUrl currentUri = it[QLatin1String("uri")].uri();

        if (set.keys().contains(currentUri)) {
            person = set.value(currentUri);
        } else {
            person = new PersonCacheItem(currentUri);
            set.insert(currentUri, person);
            //person->addFacet(facetType);
        }

        Q_FOREACH(const QUrl &keyUri, requestedKeys) {
            keyString = keyUri.toString();
            //convert every key to correspond to the nepomuk bindings
            //FIXME: get this method out of the loop, it repeats too many times for nothing
            keyString = keyString.right(keyString.length() - keyString.lastIndexOf(QLatin1Char('/')) - 1).replace(QLatin1Char('#'), QLatin1Char('_'));

            person->addData(keyUri, it[keyString].toString());
            kDebug() << keyUri << it[keyString].toString();
            //d_ptr->allRequestedKeys.insert(facetType, keyUri);
        }

    }

    d_ptr->persons.unite(set);

    d_ptr->model = new PersonsModel(&d_ptr->persons);

    // FIXME: Connect up signals/slots so that when the nepomuk context signals something has
    //        happened we relay it to the model

}

PersonsModel* PersonCache::model()
{
    return d_ptr->model;
}

void PersonCache::onNewPersonCreated(Nepomuk::Resource res, QList<QUrl> types)
{
    Q_D(PersonCache);
    PersonCacheItem *person = new PersonCacheItem(res.uri());

    kDebug() << "New person created in Nepomuk";

    bool facetComplete = true;
    QList<PersonCacheItem::FacetType> keys = d->allRequestedKeys.uniqueKeys();
    Q_FOREACH (const PersonCacheItem::FacetType &facet, keys ) {
        QList<QUrl> values = d->allRequestedKeys.values(facet);
        Q_FOREACH (const QUrl &keyUri, values) {
            if (res.hasProperty(keyUri)) {
                if (res.property(keyUri).isString()) {
                    person->addData(keyUri, res.property(keyUri).toString());
                } else if (res.property(keyUri).isStringList()) {
                    person->addData(keyUri, res.property(keyUri).toStringList());
                }
            }
        }
    }

    d->persons.insert(res.uri(), person);

    emit personAddedToCache(person);
}

void PersonCache::onPersonRemoved()
{
    //FIXME: implement me
}

void PersonCache::onPersonPropertyChanged(Nepomuk::Resource res, Nepomuk::Types::Property property, QVariant value)
{
    //FIXME: implement me
}

#include "person-cache.moc"
