/*
    KPeople - Duplicates
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

#ifndef MATCH_H
#define MATCH_H

#include <QList>
#include <QPersistentModelIndex>
#include <QtCore/qnamespace.h>

#include "backends/abstractcontact.h"
#include "kpeople/kpeople_export.h"

namespace KContacts
{
    class Addressee;
}

namespace KPeople
{

class KPEOPLE_EXPORT Match
{
Q_GADGET
public:
    enum MatchReason {
        NameMatch,
        EmailMatch
    };
    Q_ENUMS(MatchReason);

    Match() {}
    Match(const QList<MatchReason>& roles, const QPersistentModelIndex& a, const QPersistentModelIndex& b);
    bool operator==(const Match &m) const;
    bool operator<(const Match &m) const;
    QStringList matchReasons() const;
    static QString matchValue(MatchReason r, const AbstractContact::Ptr &addr);
    static QList<Match::MatchReason> matchAt(const AbstractContact::Ptr &value, const AbstractContact::Ptr &toCompare);

    QList<MatchReason> reasons;
    QPersistentModelIndex indexA, indexB;
};

}

Q_DECLARE_METATYPE(KPeople::Match);

#endif // MATCH_H
