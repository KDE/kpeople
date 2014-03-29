/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  David Edmundson <david@davidedmundson.co.uk>
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

#include "mergetests.h"

#include "personmanager_p.h"

#include <QtTest>

QTEST_MAIN(MergeTests);

void MergeTests::initTestCase()
{
    PersonManager::instance("/tmp/kpeople_test_db");
}

void MergeTests::cleanupTestCase()
{
    QFile::remove("/tmp/kpeople_test_db");
}

void MergeTests::init()
{
    // Called before each testfunction is executed
}

void MergeTests::cleanup()
{
    // Called after every testfunction
}

void MergeTests::mergeContacts()
{
    QStringList contacts;
    contacts << "test1://contact1" << "test1://contact2";

    QSignalSpy spy(PersonManager::instance(), SIGNAL(contactAddedToPerson(QString, QString)));
    PersonManager::instance()->mergeContacts(contacts);
    QTest::qWait(50); // wait for dbus to process
    //should emit contactAddedToPerson contact1 contact2 to the same personID
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy[0][1].toString(), spy[1][1].toString());
}

void MergeTests::mergeContactAndPerson()
{
    QStringList contacts;
    contacts << "test2://contact3" << "test2://contact4";
    QSignalSpy spy(PersonManager::instance(), SIGNAL(contactAddedToPerson(QString, QString)));
    QString personId = PersonManager::instance()->mergeContacts(contacts);
    QTest::qWait(150); // wait for dbus to process

    contacts = QStringList();
    contacts << personId << "test2://contact5";
    QSignalSpy spy2(PersonManager::instance(), SIGNAL(contactAddedToPerson(QString, QString)));
    PersonManager::instance()->mergeContacts(contacts);
    QTest::qWait(150); // wait for dbus to process
    QCOMPARE(spy2.count(), 1);

    //expect contact contactAddedToPerson(contact5, person1)

    QCOMPARE(spy2[0][0].toString(), QString("test2://contact5"));
    QCOMPARE(spy2[0][1].toString(), personId);
}

void MergeTests::mergeAlreadyMergedContact()
{
    //merge contact1 and contact2
    QStringList contacts;
    contacts << "test3://contact1" << "test3://contact2";
    QString person1 = PersonManager::instance()->mergeContacts(contacts);
    QTest::qWait(150); // wait for dbus to process



    //merge contact2 and contact3

    //contact 2 is already merged. It should error

    QStringList contacts2;
    contacts2 << "test3://contact2" << "test3://contact3";

    QSignalSpy addedSpy(PersonManager::instance(), SIGNAL(contactAddedToPerson(QString, QString)));
    QSignalSpy removedSpy(PersonManager::instance(), SIGNAL(contactRemovedFromPerson(QString)));
    QString person2 = PersonManager::instance()->mergeContacts(contacts2);
    QTest::qWait(150); // wait for dbus to process
    QCOMPARE(removedSpy.count(), 0);
    QCOMPARE(addedSpy.count(), 0);
    QCOMPARE(person2, QString());

    //check no changes occurred
    QCOMPARE(PersonManager::instance()->contactsForPersonId(person1), QStringList() << "test3://contact1" << "test3://contact2");
}




#include "mergetests.moc"
