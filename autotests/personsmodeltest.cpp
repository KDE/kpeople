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
    PersonManager::instance()->mergeContacts(QStringList() << QStringLiteral("fakesource://contact2") << QStringLiteral("fakesource://contact3"));

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
    QCOMPARE(m_model->rowCount(), 2);
    QCOMPARE(m_model->data(m_model->index(0)).toString(), QStringLiteral("Person A"));
    QCOMPARE(m_model->data(m_model->index(1)).toString(), QStringLiteral("Contact 1"));

    m_source->changeProperty(AbstractContact::NameProperty, QStringLiteral("Contact 2"));

    QCOMPARE(m_model->rowCount(), 2);
    QCOMPARE(m_model->data(m_model->index(0)).toString(), QStringLiteral("Person A"));
    QCOMPARE(m_model->data(m_model->index(1)).toString(), QStringLiteral("Contact 2"));
}
