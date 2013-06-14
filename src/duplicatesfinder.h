/*
    Persons Model
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
#include "kpeople_export.h"
#include "match.h"

class PersonsModel;
class KPEOPLE_EXPORT DuplicatesFinder : public KJob
{
    Q_OBJECT
    public:
        explicit DuplicatesFinder(PersonsModel *model, QObject *parent = 0);

        QList<Match> results() const;

    public Q_SLOTS:
        virtual void start();

    private Q_SLOTS:
        void doSearch();

    private:
        QList< int > matchAt(const QVariantList &value, const QVariantList &toCompare) const;
        PersonsModel *m_model;
        QList<Match> m_matches;
        QVector<int> m_compareRoles;
};

#endif // DUPLICATESFINDER_H
