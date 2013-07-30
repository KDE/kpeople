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

#ifndef MODELTESTS_H
#define MODELTESTS_H

#include "../lib/testbase.h"
#include "personsmodel.h"

#include <QObject>
#include <QUrl>

class ModelTests : public Nepomuk2::TestBase
{
    Q_OBJECT
private slots:
    void init();

    void addContactToModel();
    void removeContactFromModel();
    void addPersonToModel();
//     void removePersonFromModel();
    void mergeContacts();

private:
    QUrl m_contact1Uri;
    QUrl m_contact2Uri;
    QUrl m_contact3Uri;
    QUrl m_personAUri;

    QUrl m_contact1EmailUri;
    QUrl m_contact2EmailUri;

    void addContact1();
    void addContact2();
    void removeContact1();
    void removeContact2();

    void addPerson();
};

Q_DECLARE_METATYPE(QModelIndex)

#endif // MODELTESTS_H
