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

#include "personsmodeltest.h"

#include <QtTest>
#include <QFile>

//private includes
#include "personmanager_p.h"
#include "personpluginmanager_p.h"

//public kpeople includes
#include <personsmodel.h>

#include "fakecontactsource.h"

QTEST_GUILESS_MAIN(PersonsModelTest);

//the fake datasource is configured with

//ID       - NAME        - email
//contact1 - Contact1    - contact1@example.com
//contact2 - PersonA     - contact2@example.com
//contact3 - PersonA     - contact3@example.com

using namespace KPeople;

void PersonsModelTest::initTestCase()
{
    // Called before the first testfunction is executed
    PersonManager::instance(QStringLiteral("/tmp/kpeople_test_db"));

    m_source = new FakeContactSource(0); //don't own. PersonPluginManager removes it on destruction
    QHash<QString, BasePersonsDataSource *> sources;
    sources[QStringLiteral("fakesource")] = m_source;
    PersonPluginManager::setDataSourcePlugins(sources);

    m_model = new KPeople::PersonsModel(this);
}

void PersonsModelTest::cleanupTestCase()
{
    // Called after the last testfunction was executed
    QFile::remove(QStringLiteral("/tmp/kpeople_test_db"));
}

void PersonsModelTest::loadModel()
{
    QCOMPARE(m_model->rowCount(), 4);
    QCOMPARE(m_model->data(m_model->index(0)).toString(), QStringLiteral("Contact 1"));
    QCOMPARE(m_model->data(m_model->index(1)).toString(), QStringLiteral("Contact 2"));
    QCOMPARE(m_model->data(m_model->index(2)).toString(), QStringLiteral("Contact 3"));
    QCOMPARE(m_model->data(m_model->index(3)).toString(), QStringLiteral("Contact 4"));

    m_source->changeProperty(AbstractContact::NameProperty, QStringLiteral("Contact A"));

    QCOMPARE(m_model->rowCount(), 4);
    QCOMPARE(m_model->data(m_model->index(0)).toString(), QStringLiteral("Contact A"));
    QCOMPARE(m_model->data(m_model->index(1)).toString(), QStringLiteral("Contact 2"));
    QCOMPARE(m_model->data(m_model->index(2)).toString(), QStringLiteral("Contact 3"));
    QCOMPARE(m_model->data(m_model->index(3)).toString(), QStringLiteral("Contact 4"));
}

void PersonsModelTest::mergeContacts()
{
    QStringList uris{QStringLiteral("fakesource://contact1"), QStringLiteral("fakesource://contact2")};

    QCOMPARE(m_model->rowCount(), 4);
    QString newUri = KPeople::mergeContacts(uris);
    QCOMPARE(newUri, QStringLiteral("kpeople://1"));
    // TODO: replace with actual model signals spying
    QTest::qWait(2000); //give it a bit of time to update the model
    QCOMPARE(m_model->rowCount(), 3);

    QStringList uris2{QStringLiteral("fakesource://contact3"), newUri};
    QString newUri2 = KPeople::mergeContacts(uris2);
    QCOMPARE(newUri2, QStringLiteral("kpeople://1"));
    QTest::qWait(2000);
    QCOMPARE(m_model->rowCount(), 2);
}

void PersonsModelTest::unmergeContacts()
{
    QCOMPARE(m_model->rowCount(), 2);
    KPeople::unmergeContact(QStringLiteral("fakesource://contact3"));
    QTest::qWait(3000);
    QCOMPARE(m_model->rowCount(), 3);
    KPeople::unmergeContact(QStringLiteral("kpeople://1"));
    QTest::qWait(3000);
    QCOMPARE(m_model->rowCount(), 4);
}
