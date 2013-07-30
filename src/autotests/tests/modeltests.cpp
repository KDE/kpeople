/*
 *  Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "modeltests.h"
#include "personsmodelfeature.h"

#include <qtest_kde.h>

#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>

#include <Soprano/Vocabulary/NAO>

#include <QMetaType>
#include <QModelIndex>

QTEST_KDEMAIN(ModelTests, NoGUI);

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;
using namespace KPeople;

void ModelTests::init()
{
    addContact1();

}

// void ModelTests::cleanup()
// {
//     removeContact1();
// }

void ModelTests::addContactToModel()
{
    PersonsModel m;
    m.startQuery(QList<PersonsModelFeature>() << PersonsModelFeature::emailModelFeature(PersonsModelFeature::Mandatory)
                                              << PersonsModelFeature::fullNameModelFeature(PersonsModelFeature::Optional));
    QTest::kWaitForSignal(&m, SIGNAL(modelInitialized()));

    // check the model
    QCOMPARE(m.rowCount(), 1);

    addContact2();

    //wait for contact updating to finish
    QTest::kWaitForSignal(&m, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

    QCOMPARE(m.rowCount(), 2);
    QCOMPARE(m.indexForUri(m_contact2Uri).data(Qt::DisplayRole).toString(), QString("Contact Two"));
}

void ModelTests::removeContactFromModel()
{
    PersonsModel m;
    m.startQuery(QList<PersonsModelFeature>() << PersonsModelFeature::emailModelFeature(PersonsModelFeature::Mandatory));
    QTest::kWaitForSignal(&m, SIGNAL(modelInitialized()));

    // check the model
    QCOMPARE(m.rowCount(), 1);

    removeContact1();

    QCOMPARE(m.rowCount(), 0);

}

void ModelTests::addPersonToModel()
{
   //TODO: create addPerson method where a person with two grounding occurances is inserted into Nepomuk
    PersonsModel m;
    m.startQuery(QList<PersonsModelFeature>() << PersonsModelFeature::emailModelFeature(PersonsModelFeature::Mandatory));
    QTest::kWaitForSignal(&m, SIGNAL(modelInitialized()));

    qRegisterMetaType<QModelIndex>("QModelIndex");
    QSignalSpy rowsInsertedSpy(&m, SIGNAL(rowsInserted(QModelIndex,int,int)));

    qDebug() << "######################x adding new person";
    addPerson();

    for (int i = 0; i < rowsInsertedSpy.count(); i++) {
//         qDebug() << rowsInsertedSpy.at(i);
        QModelIndex a = qvariant_cast<QModelIndex>(rowsInsertedSpy.at(i).at(0));
        if (a.isValid()) {
            qDebug() << a.row() << a.parent();
            qDebug() << a.data(Qt::DisplayRole) << a.data(PersonsModel::UriRole) << m.rowCount(a);
        } else if (a.row() == -1) {
            qDebug() << "Toplevel index";
        }
    }
    qDebug() << rowsInsertedSpy.count() << m.rowCount();
    QTest::kWaitForSignal(&m, SIGNAL(rowsInserted(QModelIndex,int,int)));

    QCOMPARE(m.rowCount(), 2);
    QCOMPARE(m.rowCount(m.indexForUri(m_personAUri)), 2);

}

// void ModelTests::removePersonFromModel()
// {
//    //TODO: create removePerson method where a person is removed, ideally both with and without the contact
//    //      (without the contact it should move the contact to new fake person)
// }
//
void ModelTests::mergeContacts()
{
    addContact2();

    PersonsModel m;
    m.startQuery(QList<PersonsModelFeature>() << PersonsModelFeature::emailModelFeature(PersonsModelFeature::Mandatory));
    QTest::kWaitForSignal(&m, SIGNAL(modelInitialized()));

    QCOMPARE(m.rowCount(), 2);

    QSignalSpy rowsInsertedSpy(&m, SIGNAL(rowsInserted(QModelIndex,int,int)));

    Nepomuk2::StoreResourcesJob *job = qobject_cast<Nepomuk2::StoreResourcesJob*>(PersonsModel::createPersonFromUris(QList<QUrl>() << m_contact1Uri << m_contact2Uri));
    job->exec();
    QVERIFY(!job->error());

    QUrl personUri = job->mappings().values().first();

    if (rowsInsertedSpy.count() == 0) {
        QTest::kWaitForSignal(&m, SIGNAL(rowsInserted(QModelIndex,int,int)));
    }

    // compare the uris
    QCOMPARE(m.data(m.index(0,0), PersonsModel::UriRole).toUrl(), personUri);
    // check there's exactly one item in the model
    QCOMPARE(m.rowCount(), 1);
    // check the person item has 2 child contacts
    QCOMPARE(m.rowCount(m.index(0,0)), 2);
    // check if the child contact uris are there
    QVERIFY(m.data(m.index(0,0), PersonsModel::ChildContactsUriRole).toList().contains(m_contact1Uri));
    QVERIFY(m.data(m.index(0,0), PersonsModel::ChildContactsUriRole).toList().contains(m_contact2Uri));

    //=== unmerge part ===

    QSignalSpy rowsRemovedSpy(&m, SIGNAL(rowsRemoved(QModelIndex,int,int)));

    PersonsModel::unlinkContactFromPerson(personUri, QList<QUrl>() << m_contact1Uri);

    // wait for the person item to be removed
    // the model does three "remove rows" operations - one for the unlinked contact,
    // one for the other contact that gets reparented when we're removing the person item
    // (which is done using takeRow() && appendRow(), which QSIModel signals with rowsRemoved/Inserted)
    // and one for the person item that gets removed
    while (rowsRemovedSpy.count() < 3) {
        QTest::kWaitForSignal(&m, SIGNAL(rowsRemoved(QModelIndex,int,int)));
    }

    // the model should now have two rows, each should be a fake person
    QCOMPARE(m.rowCount(), 2);

    // the person had two contacts, if just one is left, the person is supposed to be removed,
    // as such there must not be an item with that person's uri anymore
    QVERIFY(!m.indexForUri(personUri).isValid());
}

void ModelTests::addContact1()
{
    Nepomuk2::SimpleResourceGraph graph;

    Nepomuk2::SimpleResource contact;
    contact.addType(NCO::PersonContact());
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
    m_contact1EmailUri = job->mappings()[email.uri()];
}

void ModelTests::addContact2()
{
    Nepomuk2::SimpleResourceGraph graph;

    Nepomuk2::SimpleResource contact;
    contact.addType(NCO::PersonContact());
    contact.addProperty(NCO::fullname(), "Contact Two");

    Nepomuk2::SimpleResource email;
    email.addType(NCO::EmailAddress());
    email.addProperty(NCO::emailAddress(), "contact2@example.com");

    contact.addProperty(NCO::hasEmailAddress(), email);

    graph << contact << email;

    Nepomuk2::StoreResourcesJob *job = graph.save();
    job->exec();

    if (job->error()) {
        qWarning() << job->errorString();
        QVERIFY(!job->error());
    }
    m_contact2Uri = job->mappings()[contact.uri()];
    m_contact2EmailUri = job->mappings()[email.uri()];
}

void ModelTests::removeContact1()
{
    KJob *job = Nepomuk2::removeResources(QList<QUrl>() << m_contact1Uri << m_contact1EmailUri);
    job->exec();

    if (job->error()) {
        qWarning() << job->errorString();
        QVERIFY(!job->error());
    }

    m_contact1Uri = QUrl();
    m_contact1EmailUri = QUrl();
}

void ModelTests::removeContact2()
{
    KJob *job = Nepomuk2::removeResources(QList<QUrl>() << m_contact2Uri << m_contact2EmailUri);
    job->exec();

    if (job->error()) {
        qWarning() << job->errorString();
        QVERIFY(!job->error());
    }

    m_contact2Uri = QUrl();
    m_contact2EmailUri = QUrl();
}

void ModelTests::addPerson()
{
    Nepomuk2::SimpleResourceGraph graph;

    Nepomuk2::SimpleResource contact1;
    contact1.addType(NCO::PersonContact());
    contact1.addProperty(NCO::fullname(), "Contact A");

    Nepomuk2::SimpleResource email1;
    email1.addType(NCO::EmailAddress());
    email1.addProperty(NCO::emailAddress(), "contactA@example.com");

    contact1.addProperty(NCO::hasEmailAddress(), email1);


    Nepomuk2::SimpleResource contact2;
    contact2.addType(NCO::PersonContact());
    contact2.addProperty(NCO::fullname(), "Contact B");

    Nepomuk2::SimpleResource email2;
    email2.addType(NCO::EmailAddress());
    email2.addProperty(NCO::emailAddress(), "contactB@example.com");

    contact2.addProperty(NCO::hasEmailAddress(), email2);


    Nepomuk2::SimpleResource person;
    person.addType(PIMO::Person());
    person.addProperty(PIMO::groundingOccurrence(), contact1);
    person.addProperty(PIMO::groundingOccurrence(), contact2);

    graph << person << contact1 << email1 << contact2 << email2;

    Nepomuk2::StoreResourcesJob *job = graph.save();
    job->exec();

    if (job->error()) {
        qWarning() << job->errorString();
        QVERIFY(!job->error());
    }

    m_personAUri = job->mappings()[person.uri()];
//     m_contact2Uri = job->mappings()[contact2.uri()];
//     m_contact2EmailUri = job->mappings()[email2.uri()];
}

#include "modeltests.moc"
