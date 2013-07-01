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

#include "duplicatesfinder.h"
#include "personsmodel.h"
#include <QDebug>

using namespace KPeople;

DuplicatesFinder::DuplicatesFinder(PersonsModel *model, QObject *parent)
    : KJob(parent)
    , m_model(model)
{
    m_compareRoles << PersonsModel::FullNamesRole
                   << PersonsModel::NicknamesRole
                   << PersonsModel::EmailsRole
                   << PersonsModel::PhonesRole;
}

void DuplicatesFinder::start()
{
    QMetaObject::invokeMethod(this, "doSearch", Qt::QueuedConnection);
}

//TODO: start providing partial results so that we can start processing matches while it's not done
void DuplicatesFinder::doSearch()
{
    //NOTE: This can probably optimized. I'm just trying to get the semantics right at the moment
    //maybe using nepomuk for the matching would help?

    QVector<QVariantList> collectedValues;
    m_matches.clear();

    for (int i = 0; i < m_model->rowCount(); i++) {
        QModelIndex idx = m_model->index(i, 0);

        //we gather the values
        QVariantList values;
        Q_FOREACH (int role, m_compareRoles) {
            values += idx.data(role);
        }
        Q_ASSERT(values.size() == m_compareRoles.size());

        //we check if it matches
        int j = 0;
        Q_FOREACH (const QVariantList &valueToCompare, collectedValues) {
            QList<int> matchedRoles = matchAt(values, valueToCompare);

            if (!matchedRoles.isEmpty()) {
                QPersistentModelIndex i1(m_model->index(i, 0));
                QPersistentModelIndex i2(m_model->index(j, 0));

                m_matches.append(Match(matchedRoles, i1, i2));
            }
            j++;
        }

        //we add our data for comparing later
        collectedValues.append(values);
    }
    emitResult();
}

QList<int> DuplicatesFinder::matchAt(const QVariantList &value, const QVariantList &toCompare) const
{
    Q_ASSERT(value.size() == toCompare.size());

    QList<int> ret;
    for (int i = 0; i < toCompare.size(); i++) {
        const QVariant &v = value[i];
        bool add = false;

        if (v.type() == QVariant::List) {
            QVariantList listA = v.toList();
            QVariantList listB = toCompare[i].toList();

            if (!listA.isEmpty() && !listB.isEmpty()) {
                Q_FOREACH (const QVariant &v, listB) {
                    if (!v.isNull() && listA.contains(v)) {
                        Q_ASSERT(!ret.contains(m_compareRoles[i]) && "B");
                        add = true;
                        break;
                    }
                }
            }
        } else if (!v.isNull() && v == toCompare[i]
            && (v.type() != QVariant::String || !v.toString().isEmpty()))
        {
            Q_ASSERT(!ret.contains(m_compareRoles[i]) && "A");
            add = true;
        }

        if (add) {
            ret += m_compareRoles[i];
        }
    }
    return ret;
}

QList<Match> DuplicatesFinder::results() const
{
    return m_matches;
}
