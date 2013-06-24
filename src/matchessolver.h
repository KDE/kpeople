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

#ifndef MATCHESSOLVER_H
#define MATCHESSOLVER_H

#include <KJob>
#include "kpeople_export.h"
#include <QSet>

class QUrl;
class QModelIndex;

namespace KPeople
{
class PersonsModel;
struct Match;

class KPEOPLE_EXPORT MatchesSolver : public KJob
{
    Q_OBJECT
    public:
        explicit MatchesSolver(const QList<Match> &matches, PersonsModel *model, QObject *parent = 0);
        virtual void start();

    public Q_SLOTS:
        void startMatching();
    private Q_SLOTS:
        void jobDone(KJob *job=0);

    private:
        QList<QUrl> contactUris(const QModelIndex &idxOrigin);

        QList<Match> m_matches;
        QSet<KJob*> m_pending;
        PersonsModel *m_model;
};
}

#endif // MATCHESSOLVER_H
