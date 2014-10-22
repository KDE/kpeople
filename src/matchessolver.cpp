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

#include "matchessolver_p.h"
#include "match_p.h"
#include "personsmodel.h"
#include "personmanager_p.h"
#include <QString>
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
    QHash<QString, QSet<QString> > jobsData;
    // has a relation of each person, to know where it is
    QHash<QString, QString> destinationResolver;
    Q_FOREACH(const Match &m, m_matches) {
        QString urlA = m.indexA.data(PersonsModel::PersonIdRole).toString();
        QString urlB = m.indexB.data(PersonsModel::PersonIdRole).toString();
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
            QList<QString> keys = destinationResolver.keys(urlB);
            foreach(const QString& key, keys) {
                destinationResolver[key] = urlA;
            }
        } else {
            //if A is in but not B, we want B wherever A is
            if(inA) {
                qSwap(urlB, urlA);
            }
            //in case B is anywhere, add A to that set, otherwise just insert B
            const QString mergeUrl = destinationResolver.value(urlB, urlB);

            QSet<QString>& jobs = jobsData[mergeUrl];
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

    Q_FOREACH(const QSet<QString>& uris, jobsData) {
        if (PersonManager::instance()->mergeContacts(uris.toList()).isEmpty()) {
            qWarning() << "error: failing to merge contacts: " << uris;
        }
    }
    emitResult();
}
