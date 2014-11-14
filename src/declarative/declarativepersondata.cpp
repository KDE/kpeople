/*
    Copyright (C) 2013  David Edmundson <D.Edmundson@lboro.ac.uk>

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


#include "declarativepersondata.h"

#include <QDebug>
#include <KPeople/KPeople/PersonData>

DeclarativePersonData::DeclarativePersonData(QObject *parent)
    : QObject(parent)
    , m_person(nullptr)
{
}

void DeclarativePersonData::setPersonId(const QString& id)
{
    m_id = id;
    delete m_person;
    m_person = new KPeople::PersonData(id, this);

    emit personChanged();
}

KPeople::PersonData* DeclarativePersonData::person() const
{
    return m_person;
}

QString DeclarativePersonData::personId() const
{
    return m_id;
}
