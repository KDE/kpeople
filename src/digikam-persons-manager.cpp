/*
    Digikam persons manager
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "digikam-persons-manager.h"

#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/NCO>

#include "person-cache.h"
#include "abstract-persons-manager_p.h"
#include "person-cache-item-set.h"
#include "digikam-person-cache-item-facet.h"
#include "digikam-persons-model.h"

class DigikamPersonsManagerPrivate : public AbstractPersonsManagerPrivate {
public:
    DigikamPersonsModel *model;
    QHash<QUrl, DigikamPersonCacheItemFacet*> data;
};

DigikamPersonsManager::DigikamPersonsManager(PersonCache *pc, QObject *parent)
  : AbstractPersonsManager(pc, parent), d_ptr(new DigikamPersonsManagerPrivate)
{
    Q_D(DigikamPersonsManager);
    connect(pc, SIGNAL(personAddedToCache(PersonCacheItem*)),
            this, SLOT(onPersonAddedToCache(PersonCacheItem*)));

    d->requestedKeys << Soprano::Vocabulary::NAO::prefLabel()
//                      << Soprano::Vocabulary::NAO::prefSymbol()
//                      << Nepomuk::Vocabulary::NCO::imNickname()
//                      << Nepomuk::Vocabulary::NCO::imAccountType()
//                      << Nepomuk::Vocabulary::NCO::imID()
    //                      << Nepomuk::Vocabulary::Telepathy::statusType()
    //                      << Nepomuk::Vocabulary::Telepathy::accountIdentifier()
                        << QUrl(QLatin1String("http://nepomuk.kde.org/ontologies/2009/06/20/telepathy#statusType"))
                        << QUrl(QLatin1String("http://nepomuk.kde.org/ontologies/2009/06/20/telepathy#accountIdentifier"))
//     << Nepomuk::Vocabulary::NCO::imStatus()
                        << Nepomuk::Vocabulary::NCO::hasEmailAddress();

    QString query = QLatin1String("select distinct ?uri ?nao_prefLabel ?pimo_groundingOccurrence ?nco_hasIMAccount"
    " ?telepathy_statusType ?nco_imID ?nco_hasEmailAddress"
    " ?nao_prefSymbol ?telepathy_accountIdentifier"

                                  "WHERE { ?uri a pimo:Person ."

    " ?uri                       pimo:groundingOccurrence    ?pimo_groundingOccurrence . "
    "OPTIONAL { ?pimo_groundingOccurrence  nco:hasIMAccount            ?nco_hasIMAccount . } "
    "OPTIONAL { ?nco_hasIMAccount          telepathy:statusType        ?telepathy_statusType . } "
    "OPTIONAL { ?nco_hasIMAccount          nco:imID                    ?nco_imID . } "
    "OPTIONAL { ?nco_hasIMAccount          nco:isAccessedBy            ?nco_isAccessedBy . } "
    "OPTIONAL { ?nco_isAccessedBy          telepathy:accountIdentifier ?telepathy_accountIdentifier . } "

                                   "?uri                       nao:prefLabel        ?nao_prefLabel ."
    "OPTIONAL { ?pimo_groundingOccurrence  nco:hasEmailAddress  ?nco_hasEmailAddress . }"

                                   "}");

    createPersonsInterface(pc->instance()->query(query, PersonCacheItem::DigikamFacet, d->requestedKeys));
    d->model = new DigikamPersonsModel(&d->data, this);
}

DigikamPersonsManager::~DigikamPersonsManager()
{
}

void DigikamPersonsManager::onPersonAddedToCache(PersonCacheItem *person)
{
    Q_D(DigikamPersonsManager);
    if (person->hasFacet(PersonCacheItem::DigikamFacet)) {
        d->model->beginInsertData(QModelIndex(), d->data.size(), d->data.size());
        d->data.insert(person->uri(), new DigikamPersonCacheItemFacet(person));
        d->model->endInsertData();
    }
}

void DigikamPersonsManager::createPersonsInterface(PersonCacheItemSet *persons)
{
    Q_D(DigikamPersonsManager);

    QHashIterator<QUrl, PersonCacheItem*> i(persons->data());
    while (i.hasNext()) {
        i.next();
        d->data.insert(i.key(), new DigikamPersonCacheItemFacet(i.value()));
    }
}

DigikamPersonsModel* DigikamPersonsManager::model() const
{
    Q_D(const DigikamPersonsManager);
    return d->model;
}

QList<QUrl> DigikamPersonsManager::requestedKeys() const
{
    Q_D(const DigikamPersonsManager);
    return d->requestedKeys;
}
