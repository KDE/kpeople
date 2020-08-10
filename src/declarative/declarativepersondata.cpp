/*
    SPDX-FileCopyrightText: 2013 David Edmundson <D.Edmundson@lboro.ac.uk>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "declarativepersondata.h"

#include "kpeople_debug.h"
#include "../persondata.h"

DeclarativePersonData::DeclarativePersonData(QObject *parent)
    : QObject(parent)
    , m_person(nullptr)
{
}

void DeclarativePersonData::setPersonUri(const QString &id)
{
    if (id == m_id) {
        return;
    }

    m_id = id;
    delete m_person;
    if (m_id.isEmpty()) {
        m_person = nullptr;
    } else {
        m_person = new KPeople::PersonData(id, this);
    }

    emit personChanged();
}

KPeople::PersonData *DeclarativePersonData::person() const
{
    return m_person;
}

QString DeclarativePersonData::personUri() const
{
    return m_id;
}
