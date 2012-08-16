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


#include "kpeople-manager.h"
#include "person-cache.h"

class KPeopleManagerPrivate {
public:
    QList<QUrl> terms;
    QString query;
    PersonCache *pc;
};

KPeopleManager::KPeopleManager(PersonCache *pc)
    : d_ptr(new KPeopleManagerPrivate)
{
    Q_D(KPeopleManager);

    d->query = QString();
    d->terms = QList<QUrl>();
    d->pc = pc;
}

KPeopleManager::~KPeopleManager()
{

}

void KPeopleManager::setQuery(const QString &query)
{
    Q_D(KPeopleManager);
    d->query = query;
}

void KPeopleManager::setTerms(const QList<QUrl> &terms)
{
    Q_D(KPeopleManager);
    d->terms = terms;
}

void KPeopleManager::startQuery()
{
    Q_D(KPeopleManager);
    if (!d->query.isEmpty()) {
        d->pc->instance()->query(d->query, d->terms);
    }

}

PersonsModel* KPeopleManager::model()
{
    Q_D(KPeopleManager);
    return d->pc->instance()->model();
}
