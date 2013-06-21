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

#include "matchessolver.h"
#include "match.h"
#include "personsmodel.h"
#include <nepomuk2/datamanagement.h>
#include <QUrl>

MatchesSolver::MatchesSolver(const QList<Match> &matches, PersonsModel *model, QObject *parent)
    : KJob(parent)
    , m_matches(matches)
    , m_model(model)
{
}

void MatchesSolver::start()
{
    QMetaObject::invokeMethod(this, "startMatching", Qt::QueuedConnection);
}

void MatchesSolver::startMatching()
{
    Q_FOREACH(const Match &m, m_matches) {
        QPersistentModelIndex idxDestination = m.indexA;
        QPersistentModelIndex idxOrigin = m.indexB;

        QList<QUrl> persons;
        persons << idxDestination.data(PersonsModel::UriRole).toUrl();
        persons << idxOrigin.data(PersonsModel::UriRole).toUrl();

        KJob *job = Nepomuk2::mergeResources(persons);
        m_pending.insert(job);
        connect(job, SIGNAL(finished(KJob*)), SLOT(jobDone(KJob*)));
    }
    if(m_matches.isEmpty())
        jobDone(0);
}

QList<QUrl> MatchesSolver::contactUris(const QModelIndex &idxOrigin)
{
    QList<QUrl> ret;
    QModelIndex idx=idxOrigin.child(0,0);
    for (; idx.isValid(); idx = idx.sibling(idx.row() + 1, 0)) {
        ret += idx.data(PersonsModel::UriRole).toUrl();
    }
    return ret;
}

void MatchesSolver::jobDone(KJob *job)
{
    m_pending.remove(job);
    if (m_pending.isEmpty()) {
        emitResult();
    }
}
