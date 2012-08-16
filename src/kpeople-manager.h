/*
    Entry point of client application
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef KPEOPLE_MANAGER_H
#define KPEOPLE_MANAGER_H

#include "kpeople_export.h"

#include <QtCore/QObject>
#include <QUrl>

class PersonCache;
class PersonsModel;
class KPeopleManagerPrivate;

class KPEOPLE_EXPORT KPeopleManager : public QObject
{
    Q_OBJECT

public:
    KPeopleManager(PersonCache *pc);
    virtual ~KPeopleManager();

    void setQuery(const QString &query);
    void setTerms(const QList<QUrl> &terms);
    void startQuery();
    PersonsModel *model();

private:
    KPeopleManagerPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(KPeopleManager)
};

#endif // KPEOPLE_MANAGER_H
