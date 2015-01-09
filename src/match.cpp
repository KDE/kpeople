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

#include "match_p.h"
#include <KLocalizedString>

using namespace KPeople;

Match::Match(const QList<MatchReason> &reasons, const QPersistentModelIndex &a, const QPersistentModelIndex &b)
    : reasons(reasons), indexA(a), indexB(b)
{
    if (indexB<indexA) {
        qSwap(indexA, indexB);
    }
}

bool Match::operator==(const Match &m) const
{
    return reasons == m.reasons
           && indexA == m.indexA
           && indexB == m.indexB;
}

bool Match::operator<(const Match &m) const
{
    return indexA < m.indexA
           || (indexA == m.indexA && indexB < m.indexB);
}

QStringList Match::matchReasons() const
{
    QStringList ret;
    for(MatchReason r: reasons) {
        switch(r) {
            case NameMatch:
                ret += i18n("Name");
                break;
            case EmailMatch:
                ret += i18n("E-mail");
                break;
        }
    }
    return ret;
}

QString Match::matchValue(MatchReason r, const AbstractContact::Ptr &addr)
{
    switch(r) {
        case NameMatch:
#warning turn into variables
            return addr->customProperty(AbstractContact::NameProperty).toString();
        case EmailMatch:
            return addr->customProperty(AbstractContact::EmailProperty).toString();
    }
    Q_UNREACHABLE();
}

QList<Match::MatchReason> Match::matchAt(const AbstractContact::Ptr &value, const AbstractContact::Ptr &toCompare)
{
    QList<Match::MatchReason> ret;

    QVariant name = value->customProperty(AbstractContact::NameProperty);
    if (name.isValid() && name == toCompare->customProperty(AbstractContact::NameProperty))
        ret.append(Match::NameMatch);

    return ret;
}
