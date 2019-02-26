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

#include "duplicatesfinder_p.h"
#include "personsmodel.h"

#include <QUrl>
#include "kpeople_debug.h"

using namespace KPeople;

DuplicatesFinder::DuplicatesFinder(PersonsModel *model, QObject *parent)
    : KJob(parent)
    , m_model(model)
{
}

void DuplicatesFinder::setSpecificPerson(const QString &personUri)
{
    m_personUri = personUri;
}

void DuplicatesFinder::start()
{
    if (m_personUri.isEmpty()) {
        QMetaObject::invokeMethod(this, "doSearch", Qt::QueuedConnection);
    } else {
        QMetaObject::invokeMethod(this, "doSpecificSearch", Qt::QueuedConnection);
    }
}

//TODO: start providing partial results so that we can start processing matches while it's not done
void DuplicatesFinder::doSearch()
{
    //NOTE: This can probably optimized. I'm just trying to get the semantics right at the moment
    //maybe using nepomuk for the matching would help?

    QVector<AbstractContact::Ptr> collectedValues;
    m_matches.clear();

    if (m_model->rowCount() == 0) {
        qCWarning(KPEOPLE_LOG) << "finding duplicates on empty model!";
    }

    for (int i = 0, rows = m_model->rowCount(); i < rows; i++) {
        QModelIndex idx = m_model->index(i, 0);

        //we gather the values
        AbstractContact::Ptr values = idx.data(PersonsModel::PersonVCardRole).value<AbstractContact::Ptr>();

        //we check if it matches
        int j = 0;
        for (const AbstractContact::Ptr &valueToCompare : qAsConst(collectedValues)) {
            QList<Match::MatchReason> matchedRoles = Match::matchAt(values, valueToCompare);

            if (!matchedRoles.isEmpty()) {
                QPersistentModelIndex i2(m_model->index(j, 0));

                m_matches.append(Match(matchedRoles, idx, i2));
            }
            j++;
        }

        //we add our data for comparing later
        collectedValues.append(values);
    }
    emitResult();
}

void DuplicatesFinder::doSpecificSearch()
{
    m_matches.clear();

    QModelIndex idx = m_model->indexForPersonUri(m_personUri);
    AbstractContact::Ptr values = idx.data(PersonsModel::PersonVCardRole).value<AbstractContact::Ptr>();

    for (int i = 0, rows = m_model->rowCount(); i < rows; i++) {
        QModelIndex idx2 = m_model->index(i, 0);

        if (idx2.data(PersonsModel::PersonUriRole) == m_personUri) {
            continue;
        }

        AbstractContact::Ptr values2 = idx2.data(PersonsModel::PersonVCardRole).value<AbstractContact::Ptr>();
        QList<Match::MatchReason> matchedRoles = Match::matchAt(values, values2);
        if (!matchedRoles.isEmpty()) {
            m_matches.append(Match(matchedRoles, idx, idx2));
        }
    }

    emitResult();
}

QList<Match> DuplicatesFinder::results() const
{
    return m_matches;
}
