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

DuplicatesFinder::DuplicatesFinder(PersonsModel* model, QObject* parent)
    : KJob(parent)
    , m_model(model)
{}

void DuplicatesFinder::start()
{
    QVector<int> compareRoles;
    compareRoles << PersonsModel::NameRole << PersonsModel::EmailRole << PersonsModel::NickRole << PersonsModel::PhoneRole;
    
    QVector< QVariantList > collectedValues(compareRoles.size());
    QList< Match > candidates;
    
    int count = m_model->rowCount();
    for(int i=0; i<count; i++) {
        QModelIndex idx = m_model->index(i, 0);
        QVariantList values;
        
        //we search if it matches
        for(int role=0; role<compareRoles.size(); role++) {
            QVariant value = idx.data(compareRoles[role]);
            values += value;
            
            if(!value.isNull()) {
                int matchPos = matchAt(value, collectedValues, role);
                if(matchPos>=0) {
                    candidates.append(Match(role, matchPos, i));
                }
            }
        }
        
        //we add our data
        collectedValues.append(values);
    }
}

int DuplicatesFinder::matchAt(const QVariant& value, const QVector< QVariantList >& collectedValues, int role) const
{
    for(int i=0; i<m_model->rowCount(); i++) {
        const QVariant& v = collectedValues[i][role];
        if(v==value)
            return i;
    }
    return -1;
}
