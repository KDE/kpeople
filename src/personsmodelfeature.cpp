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


#include "personsmodelfeature.h"

QHash<QString, PersonsModel::Role> PersonsModelFeature::bindingsMap() const
{
    return m_bindingsMap;
}

void PersonsModelFeature::setBindingsMap(QHash<QString, PersonsModel::Role> bindingsMap)
{
    m_bindingsMap = bindingsMap;
}

void PersonsModelFeature::setQueryPart(const QString &part)
{
    m_queryPart = part;
}

QString PersonsModelFeature::queryPart() const
{
    if (m_isOptional) {
        return QString("OPTIONAL { %1 } ").arg(m_queryPart);
    } else {
        return m_queryPart;
    }
}

bool PersonsModelFeature::isOptional() const
{
    return m_isOptional;
}

void PersonsModelFeature::setOptional(bool optional)
{
    m_isOptional = optional;
}

QUrl PersonsModelFeature::watcherProperty() const
{
    return m_watcherProperty;
}

void PersonsModelFeature::setWatcherProperty(const QUrl &property)
{
    m_watcherProperty = property;
}
