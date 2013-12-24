/*
 * <one line to give the library's name and an idea of what it does.>
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

#include "autotests/personmanagertests.h"

#include "personmanager_p.h"

#include <QtTest>
#include <QSignalSpy>

QTEST_MAIN(PersonManagerTests);

void waitForSignal(QObject* object, const char* signal, int count=1, int timeout = 500)
{
    for (int i=0; i < count ; i++) {
        QEventLoop loop;
        loop.connect(object, signal, SLOT(quit()));
        QTimer::singleShot(timeout, &loop, SLOT(quit()));
        loop.exec();
    }
}


//debug helper, prints out the received signals on a signalspy
//assumes all our signals have 2 arguments
void debugSignals(const QSignalSpy &spy)
{
    for(int i=0; i< spy.count(); i++) {
        qDebug() << spy.signal() << spy[i][0] << spy[i][1];
    }
}


void PersonManagerTests::initTestCase()
{
    // Called before the first testfunction is executed

    //make sure no nonsense is left over from a dead test
    QFile::remove("/tmp/kpeople_personmanager_test_db");

}

void PersonManagerTests::cleanupTestCase()
{
    // Called after the last testfunction was executed
}

void PersonManagerTests::init()
{
    //create a new manager
    //in this test we don't actually use any of the datasources
    m_manager = new PersonManager("/tmp/kpeople_personmanager_test_db");
}

void PersonManagerTests::cleanup()
{
    // Called after every testfunction

    delete m_manager;
    m_manager = 0;

    QFile::remove("/tmp/kpeople_personmanager_test_db");

}

void PersonManagerTests::mergeContacts()
{
    //check DB is empty on startup
    QCOMPARE(m_manager->allPersons().size(), 0);

    QSignalSpy spy(m_manager, SIGNAL(contactAddedToPerson(QString, QString)));
    QString mergedId = m_manager->mergeContacts(QStringList() << "a://contact1" << "a://contact2");
    QCOMPARE(mergedId, QString("kpeople://1"));

    //we expect two signals one for each contact added
    waitForSignal(m_manager, SIGNAL(contactAddedToPerson(QString, QString)), 2);
    debugSignals(spy);
    QCOMPARE(spy.count(), 2);

    //check database updated correctly
    QMultiHash<QString, QString> people = m_manager->allPersons();
    QCOMPARE(people.count("kpeople://1"), 2);
    QCOMPARE(people.values("kpeople://1").toSet(), QSet<QString>() << "a://contact1" << "a://contact2");
}

void PersonManagerTests::mergeContactAndPerson()
{
    //create a person
    m_manager->mergeContacts(QStringList() << "a://contact1" << "a://contact2");

    waitForSignal(m_manager, SIGNAL(contactAddedToPerson(QString, QString)), 2);


    //when merging a contact to a person we simply add the contact to an existing person
    QSignalSpy spy(m_manager, SIGNAL(contactAddedToPerson(QString, QString)));
    QString mergedId = m_manager->mergeContacts(QStringList() << "kpeople://1" << "a://contact3");
    QCOMPARE(mergedId, QString("kpeople://1"));

    waitForSignal(m_manager, SIGNAL(contactAddedToPerson(QString, QString)));
    debugSignals(spy);
    QCOMPARE(spy.count(), 1);

    //check database updated correctly
    QMultiHash<QString, QString> people = m_manager->allPersons();
    QCOMPARE(people.count("kpeople://1"), 3);
    QCOMPARE(people.values("kpeople://1").toSet(), QSet<QString>() << "a://contact1" << "a://contact2" << "a://contact3");
}

void PersonManagerTests::mergePeople()
{
    m_manager->mergeContacts(QStringList() << "a://contact1" << "a://contact2");
    m_manager->mergeContacts(QStringList() << "a://contact3" << "a://contact4");
    waitForSignal(m_manager, SIGNAL(contactAddedToPerson(QString, QString)), 4);

    QSignalSpy spy(m_manager, SIGNAL(contactAddedToPerson(QString, QString)));
    QString mergeId = m_manager->mergeContacts(QStringList() << "kpeople://1" << "kpeople://2");

    qDebug() << mergeId;
    QCOMPARE(mergeId, QString("kpeople://1"));

    //we expect two signals for contact 3 and 4
    waitForSignal(m_manager, SIGNAL(contactAddedToPerson(QString, QString)), 2);
    debugSignals(spy);
    QCOMPARE(spy.count(), 2);


}

void PersonManagerTests::mergeContactInAnotherPerson()
{

}

void PersonManagerTests::unmergeContact()
{

}

void PersonManagerTests::unmergePerson()
{

}

void PersonManagerTests::mergeContactWhichIsAlreadyMerged()
{

}

void PersonManagerTests::unmergeContactWhichDoesNotExist()
{
    QSignalSpy spy(m_manager, SIGNAL(contactAddedToPerson(QString, QString)));
    bool unmergeResult = m_manager->unmergeContact("a://contact1");
    QCOMPARE(unmergeResult, false);
    waitForSignal(m_manager, SIGNAL(contactAddedToPerson(QString, QString)), 1); //this should time out
    QCOMPARE(spy.count(), 0);
}

void PersonManagerTests::unmergePersonWhichDoesNotExist()
{
    QSignalSpy spy(m_manager, SIGNAL(contactAddedToPerson(QString, QString)));
    bool unmergeResult = m_manager->unmergeContact("kpeople://1");
    QCOMPARE(unmergeResult, false);
    waitForSignal(m_manager, SIGNAL(contactAddedToPerson(QString, QString)), 1); //this should time out
    QCOMPARE(spy.count(), 0);
}

#include "autotests/personmanagertests.moc"
