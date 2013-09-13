/*
    Copyright (C) 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
    Copyright (C) 2013 David Edmundson <david@davidedmundson.co.uk>

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
#include <QDebug>

using namespace KPeople;

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
    //Will contain all the sets to be merged
    QHash<QUrl, QSet<QUrl> > jobsData;
    // has a relation of each person, to know where it is
    QHash<QUrl, QUrl> destinationResolver;
    Q_FOREACH(const Match &m, m_matches) {
        QUrl urlA = m.indexA.data(PersonsModel::UriRole).toUrl();
        QUrl urlB = m.indexB.data(PersonsModel::UriRole).toUrl();
        Q_ASSERT(urlA != urlB);

        const bool inA = destinationResolver.contains(urlA);
        const bool inB = destinationResolver.contains(urlB);
        if(inA && inB) {
            //Both are in already, so we have to merge both sets
            destinationResolver[urlB] = urlA;
            jobsData[urlA] = jobsData.take(urlB);

            //we've put all items pointed to by urlA, to the B set
            //now we re-assign anything pointing to B as pointing to A
            //because they are the same
            QList<QUrl> keys = destinationResolver.keys(urlB);
            foreach(const QUrl& key, keys) {
                destinationResolver[key] = urlA;
            }
        } else {
            //if A is in but not B, we want B wherever A is
            if(inA) {
                qSwap(urlB, urlA);
            }
            //in case B is anywhere, add A to that set, otherwise just insert B
            const QUrl mergeUrl = destinationResolver.value(urlB, urlB);

            QSet<QUrl>& jobs = jobsData[mergeUrl];
            jobs.insert(urlB);
            jobs.insert(urlA);

            //remember where urlA and urlB are
            Q_ASSERT(urlA != mergeUrl);
            destinationResolver.insert(urlA, mergeUrl);
            if (urlB != mergeUrl) {
                destinationResolver.insert(urlB, mergeUrl);
            }
        }
    }

    Q_FOREACH(const QSet<QUrl>& uris, jobsData) {
        KJob* job = m_model->createPersonFromUris(uris.toList());
        //createPersonFromUris can return null if there nothing to be done
        if (job) {
            m_pending.insert(job);
            connect(job, SIGNAL(finished(KJob*)), SLOT(jobDone(KJob*)));
        } else {
            qWarning() << "error: failing to merge contacts: " << uris;
        }
    }

    //if there are no jobs in the queue emit finished
    if(m_pending.isEmpty()) {
        jobDone(0);
    }
}

void MatchesSolver::jobDone(KJob *job)
{
    m_pending.remove(job);
    if (m_pending.isEmpty()) {
        emitResult();
    }
}
