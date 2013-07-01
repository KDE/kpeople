/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  David Edmundson <D.Edmundson@lboro.ac.uk>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "declarativepersondata.h"

#include <KDebug>

DeclarativePersonData::DeclarativePersonData(QObject* parent)
    :KPeople::PersonData(parent)
{

}

void DeclarativePersonData::classBegin()
{

}

void DeclarativePersonData::componentComplete()
{
    if (!m_uri.isEmpty()) {
        loadUri(m_uri);
    } else if (!m_contactId.isEmpty()) {
        loadContact(m_contactId);
    } else {
        kWarning() << "item has no uri or contactId set";
    }
}

void DeclarativePersonData::setContactId(const QString& contactId)
{
    m_contactId = contactId;
}

void DeclarativePersonData::setUri(const QString& uri)
{
    m_uri = uri;
}


