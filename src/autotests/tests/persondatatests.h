/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

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


#ifndef PERSONDATATESTS_H
#define PERSONDATATESTS_H

#include <QtCore/QObject>
#include <QUrl>
#include "../lib/testbase.h"

class PersonDataTests : public Nepomuk2::TestBase
{
    Q_OBJECT
private Q_SLOTS:
    void initContact1();
    void initPersonA();

    //This tests all properties on a contact
    //FIXME - not complete!
    void contactProperties();

    //this tests loading all the properties on a person made up of several contacts
    //FIXME - not complete
    void personProperties();

    //this tests loading a contact from an ID not URI i.e foo@example.com
    void contactFromContactID();

    //this tests that we can load a PIMO::Person from a given ID
    void personFromContactID();

    //this test tries to make the PersonData crash
    void miscTests();

    //tests the contact updated signal is correctly emitted
    //FIXME - not complete
//     void changed();

private:
    QUrl m_contact1Uri;
    QUrl m_contact2Uri;
    QUrl m_contact3Uri;
    QUrl m_personAUri;
};

#endif // PERSONDATATESTS
