/*
 * Copyright (C) 2015 Aleix Pol i Gonzalez <aleixpol@blue-systems.com>
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

#include "personsproxymodeltest.h"

#include <QtTest>

//private includes
#include "personmanager_p.h"

//public kpeople includes
#include <personsmodel.h>
#include <personpluginmanager.h>
#include <personssortfilterproxymodel.h>

#include "fakecontactsource.h"

QTEST_GUILESS_MAIN(PersonsProxyModelTest)

using namespace KPeople;

void PersonsProxyModelTest::initTestCase()
{
    QVERIFY(m_database.open());

    // Called before the first testfunction is executed
    PersonManager::instance(m_database.fileName());
    m_source = new FakeContactSource(nullptr); //don't own. PersonPluginManager removes it on destruction
    QHash<QString, BasePersonsDataSource *> sources;
    sources[QStringLiteral("fakesource")] = m_source;
    PersonPluginManager::setDataSourcePlugins(sources);

    m_model = new KPeople::PersonsModel(this);
    QSignalSpy modelInit(m_model, SIGNAL(modelInitialized(bool)));

    QTRY_COMPARE(modelInit.count(), 1);
    QCOMPARE(modelInit.first().at(0).toBool(), true);
}

void PersonsProxyModelTest::cleanupTestCase()
{
    // Called after the last testfunction was executed
    m_database.close();
}

void PersonsProxyModelTest::testFiltering()
{
    PersonsSortFilterProxyModel proxy;
    proxy.setSourceModel(m_model);

    QCOMPARE(proxy.rowCount(), 4);

    proxy.setRequiredProperties(QStringList() << AbstractContact::PhoneNumberProperty);

    QCOMPARE(proxy.rowCount(), 2);

    proxy.setRequiredProperties(QStringList() << AbstractContact::PhoneNumberProperty << KPeople::AbstractContact::PresenceProperty);

    QCOMPARE(proxy.rowCount(), 3);
}
