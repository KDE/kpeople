/*
 * Copyright (C) 2015  Martin Klapetek <mklapetek@kde.org>
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

#include <QtTest>

#include "servicetest.h"

#include "../src/service/service.h"
#include "fakecontactsource.h"
#include <../../opt/qt5/include/QtDBus/QDBusPendingReply>
#include "global.h"
#include "personmanager_p.h"
#include "personpluginmanager_p.h"
#include "basepersonsdatasource.h"

#include <QDBusPendingCallWatcher>
#include <QDBusConnection>
#include <QDBusInterface>

using namespace KPeople;


QTEST_GUILESS_MAIN(LookupServiceTest);

void LookupServiceTest::initTestCase()
{
    // Called before the first testfunction is executed
    PersonManager::instance(QStringLiteral("/tmp/kpeople_test_db"));

    m_source = new FakeContactSource(0); //don't own. PersonPluginManager removes it on destruction
    QHash<QString, BasePersonsDataSource *> sources;
    sources[QStringLiteral("fakesource")] = m_source;
    PersonPluginManager::setDataSourcePlugins(sources);

    // QDBusAbstractAdaptor crashes on empty parent
    m_service = new KPeople::LookupService(new QObject(0));
    // Wait for persons model to init
    QTest::qWait(500);
}

void LookupServiceTest::cleanupTestCase()
{
    // Called after the last testfunction was executed
    QFile::remove(QStringLiteral("/tmp/kpeople_test_db"));
}

void LookupServiceTest::loadService()
{
    bool rs = QDBusConnection::sessionBus().registerService(QStringLiteral("org.kde.KPeople"));
    bool ro = QDBusConnection::sessionBus().registerObject(QStringLiteral("/LookupService"), m_service, QDBusConnection::ExportAllSlots);

    QVERIFY(rs);
    QVERIFY(ro);

    QDBusInterface iface(QStringLiteral("org.kde.KPeople"), QStringLiteral("/LookupService"), QStringLiteral("org.kde.KPeople"));
    QVERIFY(iface.isValid());
}

void LookupServiceTest::queryService()
{
    // Try full number matching
    QDBusPendingCallWatcher *pendingCall = KPeople::personForContactProperty(QStringLiteral("+44123456789"), QStringLiteral("phone"));
    pendingCall->waitForFinished();

    QDBusPendingReply<QString> reply = *pendingCall;

    QVERIFY(!reply.isError());
    QCOMPARE(reply.value(), QStringLiteral("fakesource://contact3"));

    // Now try partial number match (like a number without national code)
    pendingCall = KPeople::personForContactProperty(QStringLiteral("234567"), QStringLiteral("phone"));
    pendingCall->waitForFinished();

    reply = *pendingCall;

    QVERIFY(!reply.isError());
    QCOMPARE(reply.value(), QStringLiteral("fakesource://contact3"));
}
