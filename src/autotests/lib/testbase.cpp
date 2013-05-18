/*
 * This file is part of nepomuk-testlib
 *
 * Copyright (C) 2010-12 Vishesh Handa <me@vhanda.in>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "testbase.h"
#include "nepomukservicemanagerinterface.h"

#include <QtCore/QFile>
#include <QtCore/QDir>

#include <QtDBus/QDBusConnection>
#include <QtTest>

#include <KStandardDirs>
#include <KTempDir>
#include <KDebug>

#include <Nepomuk2/ResourceManager>

#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Soprano/NodeIterator>

#include <cstdlib>

class Nepomuk2::TestBase::Private {
public:
    org::kde::nepomuk::ServiceManager* m_serviceManager;
    QString m_repoLocation;
    KTempDir m_tempDir;
};

Nepomuk2::TestBase::TestBase(QObject* parent)
    : QObject( parent ),
      d( new Nepomuk2::TestBase::Private )
{
    char* value = getenv("NEPOMUK_TESTLIB_RUNNING");
    if( !value ) {
        QString message("The Nepomuk Test Environment is not running. Running these tests without the"
                        "test environment will delete your Nepomuk database. Aborting");
        kError() << message;
        QFAIL(message.toLatin1().data());
    }

    d->m_serviceManager = new org::kde::nepomuk::ServiceManager( "org.kde.NepomukServer", "/servicemanager", QDBusConnection::sessionBus() );

    d->m_repoLocation = KStandardDirs::locateLocal( "data", "nepomuk/repository/" );

    // Stop all the other servies
    QSet<QString> services = runningServices().toSet();
    kDebug() << "Running Services : " << services;
    services.remove( "nepomukstorage" );
    services.remove( "nepomukqueryservice" );

    Q_FOREACH( const QString & service, services )
        stopService( service );
}

Nepomuk2::TestBase::~TestBase()
{
    delete d;
}

void Nepomuk2::TestBase::cleanup()
{
    resetRepository();
}

void Nepomuk2::TestBase::cleanupTest()
{
    resetRepository();
}


void Nepomuk2::TestBase::resetRepository()
{
    kDebug() << "Reseting the repository";
    QTime timer;
    timer.start();

    QString query = QString::fromLatin1("select distinct ?r where { ?r ?p ?o. "
                                        "FILTER(regex(str(?r), '^nepomuk')) . }");
    Soprano::Model * model = Nepomuk2::ResourceManager::instance()->mainModel();

    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );
    while( it.next() ) {
        model->removeAllStatements( it[0], Soprano::Node(), Soprano::Node() );
        model->removeAllStatements( Soprano::Node(), Soprano::Node(), it[0] );
    }

    kDebug() << "Time Taken: " << timer.elapsed()/1000.0 << " seconds";
}


void Nepomuk2::TestBase::waitForServiceInitialization(const QString& service)
{
    while( !isServiceRunning( service ) ) {
        QTest::qSleep( 100 );
    }

    while( !isServiceInitialized( service ) ) {
        QTest::qSleep( 200 );
        kDebug() << runningServices();
    }
}


void Nepomuk2::TestBase::startServiceAndWait(const QString& service)
{
    if( isServiceRunning( service ) )
        return;

    kDebug() << "Starting " << service << " ...";
    startService( service );
    kDebug() << "Waiting ...";
    waitForServiceInitialization( service );
}



//
// Service Manager
//

QStringList Nepomuk2::TestBase::availableServices()
{
    QDBusPendingReply< QStringList > reply = d->m_serviceManager->availableServices();
    reply.waitForFinished();
    return reply.value();
}

bool Nepomuk2::TestBase::isServiceAutostarted(const QString& service)
{
    QDBusPendingReply< bool > reply = d->m_serviceManager->isServiceAutostarted( service );
    reply.waitForFinished();
    return reply.value();

}

bool Nepomuk2::TestBase::isServiceInitialized(const QString& name)
{
    QDBusPendingReply< bool > reply = d->m_serviceManager->isServiceInitialized( name );
    reply.waitForFinished();
    return reply.value();
}

bool Nepomuk2::TestBase::isServiceRunning(const QString& name)
{
    QDBusPendingReply< bool > reply = d->m_serviceManager->isServiceRunning( name );
    reply.waitForFinished();
    return reply.value();
}

QStringList Nepomuk2::TestBase::runningServices()
{
    QDBusPendingReply< QStringList > reply = d->m_serviceManager->runningServices();
    reply.waitForFinished();
    return reply.value();
}

void Nepomuk2::TestBase::setServiceAutostarted(const QString& service, bool autostart)
{
    d->m_serviceManager->setServiceAutostarted( service, autostart );
}

bool Nepomuk2::TestBase::startService(const QString& name)
{
    QDBusPendingReply< bool > reply = d->m_serviceManager->startService( name );
    reply.waitForFinished();
    return reply.value();
}

bool Nepomuk2::TestBase::stopService(const QString& name)
{
    QDBusPendingReply< bool > reply = d->m_serviceManager->stopService( name );
    reply.waitForFinished();
    return reply.value();
}

#include "testbase.moc"

