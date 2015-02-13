/*
 * Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "persondatatests.h"

#include <QtTest>
#include <QFile>

//private includes
#include "personmanager_p.h"
#include "personpluginmanager_p.h"

//public kpeople includes
#include <persondata.h>

#include "fakecontactsource.h"

QTEST_MAIN(PersonDataTests);

//the fake datasource is configured with

//ID       - NAME        - email
//contact1 - Contact1    - contact1@example.com
//contact2 - PersonA     - contact2@example.com
//contact3 - PersonA     - contact3@example.com

using namespace KPeople;

//this tests PersonData but also implicitly tests the private classes
// - BasePersonsDataSource
// - DefaultContactMonitor
// - MetaContact

void PersonDataTests::initTestCase()
{
    // Called before the first testfunction is executed
    PersonManager::instance(QStringLiteral("/tmp/kpeople_test_db"));
    PersonManager::instance()->mergeContacts(QStringList() << QStringLiteral("fakesource://contact2") << QStringLiteral("fakesource://contact3"));

    m_source = new FakeContactSource(0); //don't own. PersonPluginManager removes it on destruction
    QHash<QString, BasePersonsDataSource *> sources;
    sources[QStringLiteral("fakesource")] = m_source;
    PersonPluginManager::setDataSourcePlugins(sources);
}

void PersonDataTests::cleanupTestCase()
{
    // Called after the last testfunction was executed
    QFile::remove(QStringLiteral("/tmp/kpeople_test_db"));
}

void PersonDataTests::init()
{
    // Called before each testfunction is executed
}

void PersonDataTests::cleanup()
{
    // Called after every testfunction
}

void PersonDataTests::loadContact()
{
    PersonData person(QStringLiteral("fakesource://contact1"));
    //in this case we know the datasource is synchronous, but we should extend the test to cope with it not being async.

    QCOMPARE(person.contactUris().size(), 1);
    QCOMPARE(person.name(), QStringLiteral("Contact 1"));
    QCOMPARE(person.allEmails(), QStringList(QStringLiteral("contact1@example.com")));
}

void PersonDataTests::loadPerson()
{
    //loading contact 2 which is already merged should return person1
    //which is both contact 2 and 3
    PersonData person(QStringLiteral("fakesource://contact2"));

    QCOMPARE(person.contactUris().size(), 2);
    QCOMPARE(person.name(), QStringLiteral("Person A"));
    QCOMPARE(person.allEmails().size(), 2);

    //convert to set as order is not important
    QCOMPARE(person.allEmails().toSet(), QSet<QString>() << QStringLiteral("contact2@example.com") << QStringLiteral("contact3@example.com"));
}

void PersonDataTests::contactChanged()
{
    PersonData person(QStringLiteral("fakesource://contact1"));

    QCOMPARE(person.allEmails().first(), QStringLiteral("contact1@example.com"));

    QSignalSpy spy(&person, SIGNAL(dataChanged()));
    m_source->changeProperty(AbstractContact::NameProperty, QStringLiteral("newaddress@yahoo.com"));
    QCOMPARE(spy.count(), 1);

    QCOMPARE(person.allEmails().first(), QStringLiteral("newaddress@yahoo.com"));
}
