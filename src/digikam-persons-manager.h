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


#ifndef DIGIKAM_PERSONS_MANAGER_H
#define DIGIKAM_PERSONS_MANAGER_H

#include <abstract-persons-manager.h>

#include "kpeople_export.h"

#include <QUrl>
#include <QList>

class PersonCache;
class PersonCacheItemSet;
class PersonCacheItem;
class DigikamPersonsModel;
class DigikamPersonsManagerPrivate;

class KPEOPLE_EXPORT DigikamPersonsManager : public AbstractPersonsManager
{
    Q_OBJECT
public:
    DigikamPersonsManager(PersonCache *pc, QObject *parent = 0);
    virtual ~DigikamPersonsManager();

    DigikamPersonsModel* model() const;
    QList<QUrl> requestedKeys() const;

private Q_SLOTS:
    virtual void onPersonAddedToCache(PersonCacheItem *person);

private:
    void createPersonsInterface(PersonCacheItemSet *persons);

    DigikamPersonsManagerPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(DigikamPersonsManager)
};

#endif // DIGIKAM_PERSONS_MANAGER_H
