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


#include "persondatatests.h"

#include <persondata.h>

#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>


#include <Soprano/Vocabulary/NAO>

#include <qtest_kde.h>
#include <QTest>


using namespace Nepomuk2;
using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;


void PersonDataTests::contactProperties()
{
    //create a simple contact with name + email
    Nepomuk2::SimpleResourceGraph graph;

    Nepomuk2::SimpleResource contact;
    contact.addType(NCO::PersonContact());
    contact.addProperty(NAO::prefLabel(), "Contact 1");
    contact.addProperty(NCO::fullname(), "Contact One");

    Nepomuk2::SimpleResource email;
    email.addType(NCO::EmailAddress());
    email.addProperty(NCO::emailAddress(), "contact1@example.com");

    contact.addProperty(NCO::hasEmailAddress(), email);

    graph << contact << email;

    Nepomuk2::StoreResourcesJob *job = graph.save();
    job->exec();

    const QUrl contactUri = job->mappings()[contact.uri()];

    QScopedPointer<PersonData> personData(new PersonData);
    personData->setUri(contactUri.toString());

    QCOMPARE(personData->name(), QLatin1String("Contact One"));
    QCOMPARE(personData->emails(), QStringList() << QLatin1String("contact1@example.com"));

}




QTEST_KDEMAIN(PersonDataTests, NoGUI)
