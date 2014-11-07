/*
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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

#ifndef DUPLICATESFINDER_H
#define DUPLICATESFINDER_H

#include <KJob>
#include <QVariantList>
#include <QVector>
#include <QUrl>
#include <kpeople/kpeople_export.h>
#include "match_p.h"

namespace KContacts
{
class Addressee;
}

namespace KPeople
{

class PersonsModel;
class KPEOPLE_EXPORT DuplicatesFinder : public KJob
{
    Q_OBJECT
    public:
        explicit DuplicatesFinder(PersonsModel *model, QObject *parent = 0);
        void setSpecificPerson(const QString &personId);

        QList<Match> results() const;

    public Q_SLOTS:
        virtual void start();

    private Q_SLOTS:
        void doSearch();
        void doSpecificSearch();

    private:
        PersonsModel *m_model;
        QList<Match> m_matches;
        QString m_personId;
};
}

#endif // DUPLICATESFINDER_H
