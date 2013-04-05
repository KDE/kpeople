/*
    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>

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


#ifndef PERSONS_MODEL_FEATURE_H
#define PERSONS_MODEL_FEATURE_H

#include <QHash>
#include <QString>

#include "persons-model.h"

class PersonsModelFeature
{
public:
    QHash<QString, PersonsModel::Role> bindingsMap() const;
    void setBindingsMap(QHash<QString, PersonsModel::Role> bindingsMap);

    QString queryPart() const;
    void setQueryPart(const QString &part);

    bool isOptional() const;
    void setOptional(bool optional);

private:
    QHash<QString, PersonsModel::Role> m_bindingsMap;
    QString m_queryPart;
    bool m_isOptional;
};

#endif // PERSONS_MODEL_FEATURE_H
