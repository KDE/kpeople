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

#ifndef LIBKPEOPLE_PERSON_CACHE_ITEM_H
#define LIBKPEOPLE_PERSON_CACHE_ITEM_H

#include <QtCore/QtGlobal>
#include <QString>
#include <QUrl>

class PersonCacheItemPrivate;

class PersonCacheItem {

    Q_DECLARE_PRIVATE(PersonCacheItem);
    Q_DISABLE_COPY(PersonCacheItem);

public:
    enum FacetType {
        BasicFacet = 0x0,
        IMFacet = 0x1,
        PIMFacet = 0x2
    };

    Q_DECLARE_FLAGS(FacetTypes, FacetType);

    PersonCacheItem(const QUrl &uri);
    virtual ~PersonCacheItem();

    QUrl uri() const;

    void addData(const QUrl &key, const QString &value);
    void addData(const QUrl &key, const QStringList &values);
    void addHashData(const QString &key, const QUrl &uri);
    void addFacet(FacetTypes facet);

protected:
    PersonCacheItemPrivate * d_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PersonCacheItem::FacetTypes)

#endif  // Header guard

