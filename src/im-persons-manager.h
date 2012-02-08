/*
    IM Persons manager
    Copyright (C) 2011  Martin Klapetek <martin.klapetek@gmail.com>

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


#ifndef IM_PERSONS_MANAGER_H
#define IM_PERSONS_MANAGER_H

#include <abstract-persons-manager.h>

#include "kpeople_export.h"

#include <QUrl>
#include <QList>

class PersonCacheItemSet;
class PersonCacheItem;
class IMPersonsModel;
class PersonCache;
class IMPersonsManagerPrivate;

class KPEOPLE_EXPORT IMPersonsManager : public AbstractPersonsManager
{
    Q_OBJECT
public:
    explicit IMPersonsManager(PersonCache *pc, QObject* parent = 0);
    ~IMPersonsManager();

    IMPersonsModel *model() const;
    QList<QUrl> requestedKeys() const;

private Q_SLOTS:
    void onPersonAddedToCache(PersonCacheItem *person);

private:
    void createPersonsInterface(PersonCacheItemSet *persons);

    IMPersonsManagerPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(IMPersonsManager)
};

#endif // IM_PERSONS_MANAGER_H
