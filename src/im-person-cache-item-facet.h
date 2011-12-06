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


#ifndef IM_PERSON_CACHE_ITEM_FACET_H
#define IM_PERSON_CACHE_ITEM_FACET_H

#include <QtCore/QtGlobal>
#include <QString>
#include <QUrl>

#include "person-cache-item.h"

class IMPersonCacheItemFacetPrivate;

class IMPersonCacheItemFacet : public PersonCacheItem {

//     Q_DECLARE_PRIVATE(IMPersonCacheItemFacet);
    Q_DISABLE_COPY(IMPersonCacheItemFacet);

public:
    IMPersonCacheItemFacet();
    virtual ~IMPersonCacheItemFacet();

    QString label() const;

private:
//     IMPersonCacheItemFacetPrivate * const d_ptr;
};

#endif // IM_PERSON_CACHE_ITEM_FACET_H
