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

using namespace KPeople;

Match::Match(const QList<int> &roles, const QPersistentModelIndex &a, const QPersistentModelIndex &b)
    : role(roles), indexA(a), indexB(b)
{
    if (indexB<indexA) {
        qSwap(indexA, indexB);
    }
}

bool Match::operator==(const Match &m) const
{
    return role == m.role
           && indexA == m.indexA
           && indexB == m.indexB;
}

bool Match::operator<(const Match &m) const
{
    return indexA < m.indexA
           || (indexA == m.indexA && indexB < m.indexB);
}
