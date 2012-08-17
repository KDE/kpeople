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
#include "persons-model.h"
#include "persons-model-item.h"
#include "persons-model-contact-item.h"

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

    QHash<QUrl, PersonsModelItem*> persons;
    QList<QUrl> allRequestedKeys;

    //FIXME: make it qpointer
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
}

PersonCache::~PersonCache()
{
    kDebug();

    delete d_ptr;
}

void PersonCache::startQuery()
{
    kDebug();

    QString nco_query = QString::fromUtf8("select distinct ?uri ?nao_prefLabel ?pimo_groundingOccurance ?nco_hasIMAccount"
                      "?nco_imNickname ?telepathy_statusType ?nco_imID ?nco_imAccountType ?nco_hasEmailAddress"
                      "?nao_prefSymbol ?telepathy_accountIdentifier ?nco_imStatus ?pimo_prefLabel ?pimo_prefSymbol"

                      "WHERE { ?uri a nco:Contact ."

                      "?uri                       nco:hasIMAccount            ?nco_hasIMAccount ."
                      "?nco_hasIMAccount          nco:imNickname              ?nco_imNickname ."
                      "?nco_hasIMAccount          telepathy:statusType        ?telepathy_statusType ."
                      "?nco_hasIMAccount          nco:imStatus                ?nco_imStatus ."
                      "?nco_hasIMAccount          nco:imID                    ?nco_imID ."
                      "?nco_hasIMAccount          nco:imAccountType           ?nco_imAccountType ."
                      "?nco_hasIMAccount          nco:isAccessedBy            ?nco_isAccessedBy ."
                      "?nco_isAccessedBy          telepathy:accountIdentifier ?telepathy_accountIdentifier ."

                      "OPTIONAL { ?pimo_groundingOccurance  pimo:groundingOccurrence    ?uri . }"
                      "OPTIONAL { ?pimo_groundingOccurance  nao:prefLabel      ?pimo_prefLabel . }"
                      "OPTIONAL { ?pimo_groundingOccurance  nao:prefSymbol     ?pimo_prefSymbol . }"

                      "OPTIONAL { ?uri            nao:prefLabel        ?nao_prefLabel . }"
                      "OPTIONAL { ?uri            nao:prefSymbol       ?nao_prefSymbol . }"
                      "OPTIONAL { ?uri            nco:hasEmailAddress  ?nco_hasEmailAddress . }"

    "}");

    QList<QUrl> list;
    list << Soprano::Vocabulary::NAO::prefLabel()
    << Soprano::Vocabulary::NAO::prefSymbol()
    << Nepomuk::Vocabulary::NCO::imNickname()
    << Nepomuk::Vocabulary::NCO::imAccountType()
    << Nepomuk::Vocabulary::NCO::imID()
    //                      << Nepomuk::Vocabulary::Telepathy::statusType()
    //                      << Nepomuk::Vocabulary::Telepathy::accountIdentifier()
    << QUrl(QLatin1String("http://nepomuk.kde.org/ontologies/2009/06/20/telepathy#statusType"))
    << QUrl(QLatin1String("http://nepomuk.kde.org/ontologies/2009/06/20/telepathy#accountIdentifier"))
    << Nepomuk::Vocabulary::NCO::imStatus()
    << Nepomuk::Vocabulary::NCO::hasEmailAddress();

    Soprano::QueryResultIterator it = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery(nco_query,
                                                                                                      Soprano::Query::QueryLanguageSparql);

    QHash<QUrl, PersonsModelItem*> set;

    QHash<QUrl, PersonsModelItem *> personNodes;
    QHash<PersonsModelItem *, QList<PersonsModelContactItem *> > contactNodes;

    while(it.next()) {
        QUrl currentUri = it[QLatin1String("uri")].uri();
        QUrl pimoPersonUri = it[QLatin1String("pimo_groundingOccurance")].uri();

        QHash< QUrl, PersonsModelItem* >::const_iterator pos = personNodes.constFind(pimoPersonUri);
        if (!pimoPersonUri.isEmpty()) {
            if (pos != personNodes.constEnd()) {
                set.insert(pimoPersonUri, pos.value());
            } else {
                pos = personNodes.insert(pimoPersonUri, new PersonsModelItem(pimoPersonUri));
            }
        } else {
            kDebug() << "Not a person" << currentUri << pimoPersonUri;
        }

        QString display = it[QLatin1String("nao_prefLabel")].toString();
        PersonsModelContactItem* contactNode = new PersonsModelContactItem(currentUri, display);

        Q_FOREACH(const QUrl &keyUri, list) {
            QString keyString = keyUri.toString();
            //convert every key to correspond to the nepomuk bindings
            //FIXME: get this method out of the loop, it repeats too many times for nothing
            keyString = keyString.right(keyString.length() - keyString.lastIndexOf(QLatin1Char('/')) - 1).replace(QLatin1Char('#'), QLatin1Char('_'));

            contactNode->addData(keyUri, it[keyString].toString());
        }

        contactNodes[pos.value()].append(contactNode);
    }

    d_ptr->persons.unite(set);

    // FIXME: Connect up signals/slots so that when the nepomuk context signals something has
    //        happened we relay it to the model

    emit contactsFetched(personNodes, contactNodes);
}

void PersonCache::onNewPersonCreated(Nepomuk::Resource res, QList<QUrl> types)
{
    Q_D(PersonCache);

    bool newPerson = false;
    PersonsModelItem *person = d->persons.value(res.resourceUri());
    if (!person) {
        person = new PersonsModelItem(res.resourceUri());
        newPerson = true;
    }

    kDebug() << "New person created in Nepomuk:" << newPerson;

//     Q_FOREACH (const QUrl &keyUri, types) {
//         if (res.hasProperty(keyUri)) {
//             person->addData(keyUri, res.property(keyUri).variant());
//         }
//     }

    if (newPerson) {
        d->persons.insert(res.resourceUri(), person);
    }

    //TODO: Shouldn't we add the contacts now?
    emit personAddedToCache(person);
}

void PersonCache::onPersonRemoved()
{
    //FIXME: implement me
    //NOTE: We need to know what person was removed
}

void PersonCache::onPersonPropertyChanged(Nepomuk::Resource res, Nepomuk::Types::Property property, QVariant value)
{
    //FIXME: implement me
}

#include "person-cache.moc"
