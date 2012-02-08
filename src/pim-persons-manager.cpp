/*
    PIM Persons manager
    Copyright (C) 2011 Martin Klapetek <martin.klapetek@gmail.com>

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


#include "pim-persons-manager.h"

#include <kpeople/person-cache.h>

#include "abstract-persons-manager_p.h"
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/NCO>
#include <ontologies/telepathy.h>

class IMPersonsManagerPrivate : public AbstractPersonsManagerPrivate {
public:
//     IMPersonsModel *model;
    QList<QUrl> requestedKeys;
};

PimPersonsManager::PimPersonsManager(PersonCache *pc, QObject *parent)
    : AbstractPersonsManager(pc, parent)
{
    Q_D(PimPersonsManager);

//     connect(pc, SIGNAL(personAddedToCache(PersonCacheItem*)),
//             this, SLOT(onPersonAddedToCache(PersonCacheItem*)));

    d->requestedKeys << Soprano::Vocabulary::NAO::prefLabel()
                     << Soprano::Vocabulary::NAO::prefSymbol()
                     << Nepomuk::Vocabulary::NCO::hasPhoneNumber()
                     << Nepomuk::Vocabulary::NCO::hasPostalAddress()
                     << Nepomuk::Vocabulary::Telepathy::statusType()
                     << Nepomuk::Vocabulary::NCO::hasEmailAddress();

    QString query = QLatin1String("select distinct ?uri ?nao_prefLabel ?pimo_groundingOccurrence ?nco_hasIMAccount"
                                    "?nco_imID ?nco_hasEmailAddress ?nco_hasPhoneNumber"
                                    "?nao_prefSymbol ?nco_hasPostalAddress"

                                    "WHERE { ?uri a pimo:Person ."

                                    "?uri                       pimo:groundingOccurrence  ?pimo_groundingOccurrence ."

                                    "OPTIONAL { ?pimo_groundingOccurrence  nco:hasEmailAddress       ?nco_hasEmailAddress . }"
                                    "OPTIONAL { ?uri                       nao:prefLabel        ?nao_prefLabel . }"
                                    "OPTIONAL { ?uri                       nao:prefSymbol       ?nao_prefSymbol . }"
                                    "OPTIONAL { ?nco_hasIMAccount          nco:imID             ?nco_imID . }"
                                    "OPTIONAL { ?pimo_groundingOccurrence  nco:hasPhoneNumber   ?nco_hasPhoneNumber . }"
                                    "OPTIONAL { ?pimo_groundingOccurrence  nco:hasPostalAddress ?nco_hasPostalAddress . }"
                                    "OPTIONAL { ?pimo_groundingOccurrence  nco:hasIMAccount     ?nco_hasIMAccount . }"
                                    "}");


    d->data = pc->instance()->query(query, PersonCacheItem::PIMFacet, d->requestedKeys);

    //     kDebug() << m_data->data().keys();

    d->model = new IMPersonsModel(d->data, 0);
}


PimPersonsManager::~PimPersonsManager()
{

}

