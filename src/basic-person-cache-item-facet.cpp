/*
 * This file is part of kpeople
 *
 * Copyright (C) 2011 Collabora Ltd. <info@collabora.co.uk>
 *   @author George Goldberg <george.goldberg@collabora.co.uk>
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

#include "basic-person-cache-item-facet.h"

#include <KDebug>


/****************************** BasicPersonCacheItemFacet::Private ********************************/

class BasicPersonCacheItemFacetPrivate {

protected:
    Q_DECLARE_PUBLIC(BasicPersonCacheItemFacet);
    BasicPersonCacheItemFacet * const q_ptr;

public:
    explicit BasicPersonCacheItemFacetPrivate(BasicPersonCacheItemFacet *q)
    : q_ptr(q)
    { }

    virtual ~BasicPersonCacheItemFacetPrivate()
    { }
};


/****************************** BasicPersonCacheItemFacet *****************************************/

BasicPersonCacheItemFacet::BasicPersonCacheItemFacet()
: d_ptr(new BasicPersonCacheItemFacetPrivate(this))
{
    kDebug();
}

BasicPersonCacheItemFacet::~BasicPersonCacheItemFacet()
{
    kDebug();

    delete d_ptr;
}

