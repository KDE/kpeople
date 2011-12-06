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
#include "person-cache.h"
#include "person-cache-item-set.h"
#include <KDebug>
#include "im-persons-model.h"


IMPersonsManager::IMPersonsManager(PersonCache *pc, QObject *parent)
    : QObject(parent)
{
    m_personCache = pc;

    QString query = QLatin1String("select distinct ?uri ?label ?go ?im ?email ?photo where { ?uri a pimo:Person ."
                                  "OPTIONAL { ?uri nao:prefLabel ?label . }"
                                  "?uri pimo:groundingOccurrence ?go ."
                                  "OPTIONAL { ?go nco:hasIMAccount ?im . }"
                                  "OPTIONAL { ?go nco:hasEmailAddress ?email . }"
                                  "OPTIONAL { ?uri nao:prefSymbol ?photo . }"
                                  "}");

    PersonCacheItemSet *set = pc->instance()->query(PersonCacheItem::IMFacet, query);

    m_model = new IMPersonsModel(set, 0);

    kDebug() << set->data().keys();
}

IMPersonsManager::~IMPersonsManager()
{

}

IMPersonsModel *IMPersonsManager::model() const
{
    return m_model;
}
