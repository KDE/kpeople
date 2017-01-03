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
#include <QSet>

#include <kpeople/kpeople_export.h>
#include "match_p.h"

class QUrl;
class QModelIndex;

namespace KPeople
{
class PersonsModel;

/**
 * @brief This class is responsible for merging different persons into one
 *
 * One of the important tasks of KPeople is putting together different contacts into
 * the same person representation.
 *
 * This class gets a list of matches, usually from @a DuplicatesFinder and makes
 * the needed changes to the database so that everything looks as it should.
 */

class KPEOPLE_EXPORT MatchesSolver : public KJob
{
    Q_OBJECT
public:
    /**
     * Constructs a solver given the @p matches against a @p model.
     *
     * Note: Obviously, the @p model should be the same @p matches refers to.
     */
    explicit MatchesSolver(const QList<Match> &matches, PersonsModel *model, QObject *parent = 0);

    /**
     * Start doing some crazy matching!
     *
     * @see KJob::finished()
     */
    void start() Q_DECL_OVERRIDE;

private Q_SLOTS:
    void startMatching();

private:
    QList<Match> m_matches;
    PersonsModel *m_model;
};
}

#endif // MATCHESSOLVER_H
