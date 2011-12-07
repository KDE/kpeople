/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Martin Klapetek <email>

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


#include "im-persons-manager.h"

#include <KDebug>

#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/NCO>

#include "person-cache.h"
#include "person-cache-item-set.h"
#include "im-persons-model.h"

IMPersonsManager::IMPersonsManager(PersonCache *pc, QObject *parent)
    : QObject(parent)
{
    m_personCache = pc;

    connect(m_personCache, SIGNAL(personAddedToCache(PersonCacheItem*)),
            this, SLOT(onPersonAddedToCache(PersonCacheItem*)));

    QList<QUrl> requestedKeys;
    requestedKeys << Soprano::Vocabulary::NAO::prefLabel()
                  << Nepomuk::Vocabulary::NCO::imNickname()
                  << Nepomuk::Vocabulary::NCO::imAccountType()
//                  << Nepomuk::Vocabulary::NCO::imStatusType()
                  << Nepomuk::Vocabulary::NCO::hasEmailAddress();

    QString query = QLatin1String("select distinct ?uri ?nao_prefLabel ?pimo_groundingOccurrence ?nco_hasIMAccount"
                                  " ?nco_imNickname ?nco_imStatusType ?nco_imID ?nco_imAccountType ?nco_hasEmailAddress"
                                  " ?nao_prefSymbol "

                                  "WHERE { ?uri a pimo:Person ."

                                  "?uri                       pimo:groundingOccurrence  ?pimo_groundingOccurrence ."
                                  "?pimo_groundingOccurrence  nco:hasIMAccount          ?nco_hasIMAccount ."
                                  "?nco_hasIMAccount          nco:imNickname            ?nco_imNickname ."
//                                  "?nco_hasIMAccount          nco:imStatusType          ?nco_imStatusType ."
                                  "?nco_hasIMAccount          nco:imID                  ?nco_imID . "
                                  "?nco_hasIMAccount          nco:imAccountType         ?nco_imAccountType ."

                                  "OPTIONAL { ?uri                       nao:prefLabel        ?nao_prefLabel . }"
                                  "OPTIONAL { ?uri                       nao:prefSymbol       ?nao_prefSymbol . }"
                                  "OPTIONAL { ?pimo_groundingOccurrence  nco:hasEmailAddress  ?nco_hasEmailAddress . }"

                                  "}");


//     select distinct ?uri ?label ?go ?im ?email ?photo ?a ?b where { ?uri a pimo:Person .
//     OPTIONAL { ?uri nao:prefLabel ?label . }
//     ?uri pimo:groundingOccurrence ?go .
//
//     ?go nco:hasIMAccount ?im .
//     ?im nco:imNickname ?a .
//     ?im nco:imID ?b .
//     OPTIONAL { ?go nco:hasEmailAddress ?email . }
//     OPTIONAL { ?uri nao:prefSymbol ?photo . }
//     }

    m_data = pc->instance()->query(query, PersonCacheItem::IMFacet, requestedKeys);

    kDebug() << m_data->data().keys();

    m_model = new IMPersonsModel(m_data, 0);
}

IMPersonsManager::~IMPersonsManager()
{

}

IMPersonsModel *IMPersonsManager::model() const
{
    return m_model;
}

void IMPersonsManager::onPersonAddedToCache(PersonCacheItem *person)
{
    //TODO: if the person fits the IM persons, add him to the set
}
