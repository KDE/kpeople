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
#include <Nepomuk2/Vocabulary/PIMO>

#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/Resource>

#include <Soprano/Vocabulary/NAO>

#include <qtest_kde.h>
#include <QTest>


using namespace Nepomuk2;
using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

void PersonDataTests::initContact1()
{
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

    if( job->error() ) {
        qWarning() << job->errorString();
        QVERIFY(!job->error());
    }

    m_contact1Uri = job->mappings()[contact.uri()];
}

void PersonDataTests::initPersonA()
{
    //create contact 2 and contact 3
    //both have a ground truth to person1

    Nepomuk2::SimpleResourceGraph graph;

    Nepomuk2::SimpleResource contact2;
    {
        contact2.addType(NCO::PersonContact());
        contact2.addProperty(NAO::prefLabel(), "Person A");
        contact2.addProperty(NCO::fullname(), "Person A");

        Nepomuk2::SimpleResource email;
        email.addType(NCO::EmailAddress());
        email.addProperty(NCO::emailAddress(), "contact2@example.com");
        contact2.addProperty(NCO::hasEmailAddress(), email);

        graph << contact2 << email;
    }

    Nepomuk2::SimpleResource contact3;
    {
        contact3.addType(NCO::PersonContact());
        contact3.addProperty(NAO::prefLabel(), "Person A");
        contact3.addProperty(NCO::fullname(), "Person A");

        Nepomuk2::SimpleResource email;
        email.addType(NCO::EmailAddress());
        email.addProperty(NCO::emailAddress(), "contact3@example.com");
        contact3.addProperty(NCO::hasEmailAddress(), email);
        graph << contact3 << email;
    }

    Nepomuk2::SimpleResource personA;
    personA.addType(PIMO::Person());
    personA.addProperty(PIMO::groundingOccurrence(), contact2);
    personA.addProperty(PIMO::groundingOccurrence(), contact3);

    graph << personA;

    Nepomuk2::StoreResourcesJob *job = graph.save();
    job->exec();

    if( job->error() ) {
        qWarning() << job->errorString();
        QVERIFY(!job->error());
    }

    m_contact2Uri = job->mappings()[contact2.uri()];
    m_contact3Uri = job->mappings()[contact3.uri()];
    m_personAUri = job->mappings()[personA.uri()];

}

void PersonDataTests::init()
{
    initContact1();
    initPersonA();
}


void PersonDataTests::contactProperties()
{
    //create a simple contact with name + email
    PersonData personData;
    personData.setUri(m_contact1Uri.toString());

    QCOMPARE(personData.uri(), m_contact1Uri.toString());
    QCOMPARE(personData.isPerson(), false);
    QCOMPARE(personData.contactResources().size(), 1);

    QCOMPARE(personData.name(), QLatin1String("Contact One"));
    QCOMPARE(personData.emails(), QStringList() << QLatin1String("contact1@example.com"));
}

void PersonDataTests::personProperties()
{
    PersonData personData;
    personData.setUri(m_personAUri.toString());
    QCOMPARE(personData.isPerson(), true);
    QCOMPARE(personData.contactResources().size(), 2);
    QCOMPARE(personData.name(), QLatin1String("Person A"));
    QCOMPARE(personData.emails(), QStringList() << QLatin1String("contact2@example.com") << QLatin1String("contact3@example.com"));
}

void PersonDataTests::personFromContactID()
{
    PersonData personData;
    personData.setContactId(QLatin1String("contact2@example.com"));
    //This should load PersonA NOT Contact2
    QCOMPARE(personData.uri(), m_personAUri.toString());
}

void PersonDataTests::contactFromContactID()
{
    PersonData personData;
    personData.setContactId(QLatin1String("contact1@example.com"));
    QCOMPARE(personData.uri(), m_contact1Uri.toString());
}

void PersonDataTests::miscTests()
{
    return;
    PersonData personData;
    personData.uri();
    personData.name();
    personData.avatar();
    personData.birthday();
    personData.contactResources();
    personData.emails();
    personData.groups();
    personData.imAccounts();
    personData.isPerson();
    personData.phones();

    PersonData test2;
    test2.setContactId(QLatin1String("IDThatDoesNotExist"));
    test2.name();
}


QTEST_KDEMAIN(PersonDataTests, NoGUI)
