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

#include "duplicatesfinder.h"
#include "persons-model.h"
#include <QDebug>

DuplicatesFinder::DuplicatesFinder(PersonsModel* model, QObject* parent)
    : KJob(parent)
    , m_model(model)
{
    m_compareRoles << PersonsModel::NameRole << PersonsModel::EmailRole << PersonsModel::NickRole << PersonsModel::PhoneRole << PersonsModel::IMRole;
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
    
    QVector< QVariantList > collectedValues;
    m_matches.clear();
    
    int count = m_model->rowCount();
    for(int i=0; i<count; i++) {
        QModelIndex idx = m_model->index(i, 0);
        
        //we gather the values
        QVariantList values;
        for(int role=0; role<m_compareRoles.size(); role++) {
            values += idx.data(m_compareRoles[role]);
        }
        Q_ASSERT(values.size()==m_compareRoles.size());
        
        //we check if it matches
        int j=0;
        foreach(const QVariantList& valueToCompare, collectedValues) {
            QList< int > matchedRoles = matchAt(values, valueToCompare);
            if(!matchedRoles.isEmpty())
                m_matches.append(Match(matchedRoles, i, j));
            j++;
        }
        
        //we add our data for comparing later
        collectedValues.append(values);
    }
    emitResult();
}

QList<int> DuplicatesFinder::matchAt(const QVariantList& value, const QVariantList& toCompare) const
{
    QList<int> ret;
    Q_ASSERT(value.size()==toCompare.size());
    for(int i=0; i<toCompare.size(); i++) {
        if(!value[i].isNull() && value[i]==toCompare[i])
            ret += m_compareRoles[i];
    }
    return ret;
}

QList< Match > DuplicatesFinder::results() const
{
    return m_matches;
}
