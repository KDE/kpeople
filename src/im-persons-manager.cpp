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

#include "ontologies/telepathy.h"

#include "person-cache.h"
#include "person-cache-item-set.h"
#include "im-persons-model.h"
#include "abstract-persons-manager_p.h"

class IMPersonsManagerPrivate : public AbstractPersonsManagerPrivate {
public:
    IMPersonsModel *model;
    QList<QUrl> requestedKeys;
};

IMPersonsManager::IMPersonsManager(PersonCache *pc, QObject *parent)
    : AbstractPersonsManager(pc, parent), d_ptr(new IMPersonsManagerPrivate)
{
    Q_D(IMPersonsManager);

    connect(pc, SIGNAL(personAddedToCache(PersonCacheItem*)),
            this, SLOT(onPersonAddedToCache(PersonCacheItem*)));

    d->requestedKeys << Soprano::Vocabulary::NAO::prefLabel()
                     << Soprano::Vocabulary::NAO::prefSymbol()
                     << Nepomuk::Vocabulary::NCO::imNickname()
                     << Nepomuk::Vocabulary::NCO::imAccountType()
                     << Nepomuk::Vocabulary::NCO::imID()
                     << Nepomuk::Vocabulary::Telepathy::statusType()
                     << Nepomuk::Vocabulary::NCO::imStatus()
                     << Nepomuk::Vocabulary::NCO::hasEmailAddress()
                     << Nepomuk::Vocabulary::Telepathy::accountIdentifier();

    QString query = QLatin1String("select distinct ?uri ?nao_prefLabel ?pimo_groundingOccurrence ?nco_hasIMAccount"
                                  " ?nco_imNickname ?telepathy_statusType ?nco_imID ?nco_imAccountType ?nco_hasEmailAddress"
                                  " ?nao_prefSymbol ?telepathy_accountIdentifier ?nco_imStatus "

                                  "WHERE { ?uri a pimo:Person ."

                                  "?uri                       pimo:groundingOccurrence    ?pimo_groundingOccurrence ."
                                  "?pimo_groundingOccurrence  nco:hasIMAccount            ?nco_hasIMAccount ."
                                  "?nco_hasIMAccount          nco:imNickname              ?nco_imNickname ."
                                  "?nco_hasIMAccount          telepathy:statusType        ?telepathy_statusType ."
                                  "?nco_hasIMAccount          nco:imStatus                ?nco_imStatus ."
                                  "?nco_hasIMAccount          nco:imID                    ?nco_imID . "
                                  "?nco_hasIMAccount          nco:imAccountType           ?nco_imAccountType ."
                                  "?nco_hasIMAccount          nco:isAccessedBy            ?nco_isAccessedBy ."
                                  "?nco_isAccessedBy          telepathy:accountIdentifier ?telepathy_accountIdentifier ."

                                  "OPTIONAL { ?uri                       nao:prefLabel        ?nao_prefLabel . }"
                                  "OPTIONAL { ?uri                       nao:prefSymbol       ?nao_prefSymbol . }"
                                  "OPTIONAL { ?pimo_groundingOccurrence  nco:hasEmailAddress  ?nco_hasEmailAddress . }"

                                  "}");


    d->data = pc->instance()->query(query, PersonCacheItem::IMFacet, d->requestedKeys);

//     kDebug() << m_data->data().keys();

    d->model = new IMPersonsModel(d->data, 0);
}

IMPersonsManager::~IMPersonsManager()
{

}

IMPersonsModel *IMPersonsManager::model() const
{
    Q_D(const IMPersonsManager);
    return d->model;
}

void IMPersonsManager::onPersonAddedToCache(PersonCacheItem *person)
{
    //TODO: if the person fits the IM persons, add him to the set
}
