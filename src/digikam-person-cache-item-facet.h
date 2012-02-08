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


#ifndef DIGIKAM_PERSON_CACHE_ITEM_FACET_H
#define DIGIKAM_PERSON_CACHE_ITEM_FACET_H

#include <QtCore/QtGlobal>
#include <QString>
#include <QUrl>

class PersonCacheItem;
class DigikamPersonCacheItemFacetPrivate;

class DigikamPersonCacheItemFacet
{
    Q_DECLARE_PRIVATE(DigikamPersonCacheItemFacet);
    Q_DISABLE_COPY(DigikamPersonCacheItemFacet);

public:
    DigikamPersonCacheItemFacet(PersonCacheItem *item);
    virtual ~DigikamPersonCacheItemFacet();

    QUrl uri() const;
    QString label() const;
    int imStatusType() const;
    QString accountId() const;
    QString contactId() const;

private:
    DigikamPersonCacheItemFacetPrivate * const d_ptr;
};

#endif // DIGIKAM_PERSON_CACHE_ITEM_FACET_H
