/*
    Duplicates Finder Auto Test
    Copyright (C) 2013  Franck Arrecot <franck.arrecot@gmail.com>

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


#include "duplicatestests.h"
#include "duplicatesfinder.h"
#include "personsmodel.h"
#include "personsmodelfeature.h"

#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/PIMO>

#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>

#include <Soprano/Vocabulary/NAO>

#include <qtest_kde.h>
#include <QTest>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;
using namespace KPeople;

void DuplicatesTests::initContact1()
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

    if (job->error()) {
        qWarning() << job->errorString();
        QVERIFY(!job->error());
    }
    m_contact1Uri = job->mappings()[contact.uri()];
}

void DuplicatesTests::initPersonA()
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

    if (job->error()) {
        qWarning() << job->errorString();
        QVERIFY(!job->error());
    }

    m_contact2Uri = job->mappings()[contact2.uri()];
    m_contact3Uri = job->mappings()[contact3.uri()];
    m_personAUri = job->mappings()[personA.uri()];

}

void DuplicatesTests::initPersonB()
{
    //create contact 2
    //both have a ground truth to person1

    Nepomuk2::SimpleResourceGraph graph;

    Nepomuk2::SimpleResource contact2BIS;
    {
        contact2BIS.addType(NCO::PersonContact());
        contact2BIS.addProperty(NAO::prefLabel(), "Person A");
        contact2BIS.addProperty(NCO::fullname(), "Person A");

        Nepomuk2::SimpleResource email;
        email.addType(NCO::EmailAddress());
        email.addProperty(NCO::emailAddress(), "contact2@example.com");
        contact2BIS.addProperty(NCO::hasEmailAddress(), email);

        graph << contact2BIS << email;
    }

    Nepomuk2::SimpleResource personB;
    personB.addType(PIMO::Person());
    personB.addProperty(PIMO::groundingOccurrence(), contact2BIS);

    graph << personB;

    Nepomuk2::StoreResourcesJob *job = graph.save();
    job->exec();

    if (job->error()) {
        qWarning() << job->errorString();
        QVERIFY(!job->error());
    }
    m_contact2BISUri = job->mappings()[contact2BIS.uri()];
    m_personBUri = job->mappings()[personB.uri()];
}

void DuplicatesTests::init()
{
    //initContact1();
    initPersonA();
    initPersonB();
}

void DuplicatesTests::duplicatesSpecificPersonSearch()
{
    PersonsModel m;
    m.startQuery(QList<PersonsModelFeature>() << PersonsModelFeature::emailModelFeature(false));
    QTest::kWaitForSignal(&m, SIGNAL(modelInitialized()));
    // check the model
    QCOMPARE(m.rowCount(),2);

    // launch the duplicates search for one Person
    QScopedPointer<DuplicatesFinder> f(new DuplicatesFinder(&m));
    f->setSpecificPerson(m_personBUri);
    f->start();
    QTest::kWaitForSignal(f.data(), SIGNAL(finished(KJob*)));

    // Duplicates found : Person A match with Person B : 1
    QList< Match > matches = f->results();
    QCOMPARE(matches.size(), 1);
    QCOMPARE(matches.first().indexA.data(PersonsModel::UriRole).toUrl() , m_personAUri);
    QCOMPARE(matches.first().indexB.data(PersonsModel::UriRole).toUrl() , m_personBUri);
}

QTEST_KDEMAIN(DuplicatesTests, NoGUI)
