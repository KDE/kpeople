/*
    Person facet used in digikam
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


#include "digikam-person-cache-item-facet.h"
#include "person-cache-item.h"

#include <KDebug>

#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/NCO>

class DigikamPersonCacheItemFacetPrivate {

public:
    PersonCacheItem *item;
};


/****************************** DigikamPersonCacheItemFacet ************************************/

DigikamPersonCacheItemFacet::DigikamPersonCacheItemFacet(PersonCacheItem *item)
    : d_ptr(new DigikamPersonCacheItemFacetPrivate())
{
    kDebug();
    d_ptr->item = item;
}

DigikamPersonCacheItemFacet::~DigikamPersonCacheItemFacet()
{
}

QUrl DigikamPersonCacheItemFacet::uri() const
{
    Q_D(const DigikamPersonCacheItemFacet);
    return d->item->uri();
}


QString DigikamPersonCacheItemFacet::label() const
{
    Q_D(const DigikamPersonCacheItemFacet);
    return d->item->data(Soprano::Vocabulary::NAO::prefLabel());
}

QString DigikamPersonCacheItemFacet::accountId() const
{
    Q_D(const DigikamPersonCacheItemFacet);
    return d->item->data(QUrl(QLatin1String("http://nepomuk.kde.org/ontologies/2009/06/20/telepathy#accountIdentifier")));
}

QString DigikamPersonCacheItemFacet::contactId() const
{
    Q_D(const DigikamPersonCacheItemFacet);
    return d->item->data(Nepomuk::Vocabulary::NCO::imID());
}

int DigikamPersonCacheItemFacet::imStatusType() const
{
    Q_D(const DigikamPersonCacheItemFacet);
    return d->item->data(QUrl(QLatin1String("http://nepomuk.kde.org/ontologies/2009/06/20/telepathy#statusType"))).toInt();
}
