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


#ifndef NEPOMUK_TESTBASE_H
#define NEPOMUK_TESTBASE_H

#include <QtCore/QObject>

#include "nepomuktest_export.h"

namespace Nepomuk2 {

    /**
     * \class TestBase testbase.h Nepomuk/TestBase
     *
     * All Nepomuk unit tests are to be derived from this class. It provides
     * an interface to manage the services.
     *
     * By default only these services are started -
     *  - nepomukstorage
     *  - nepomukqueryservice
     *
     * This class aditionally provides a mechanism to reeset the nepomuk
     * repository to its original form.
     *
     * \author Vishesh Handa <handa.vish@gmail.com>
     */
    class NEPOMUKTEST_EXPORT TestBase : public QObject
    {
        Q_OBJECT
    public:

        TestBase(QObject* parent = 0);
        virtual ~TestBase();


    private Q_SLOTS:
        virtual void cleanup();
        void cleanupTest();

    protected:
        void resetRepository();

    public Q_SLOTS:
        QStringList availableServices();
        QStringList runningServices();

        bool isServiceAutostarted(const QString &service);
        bool isServiceInitialized(const QString &name);
        bool isServiceRunning(const QString &name);
        void setServiceAutostarted(const QString &service, bool autostart);

        bool startService(const QString &name);
        bool stopService(const QString &name);

        void waitForServiceInitialization( const QString & service );
        void startServiceAndWait( const QString & service );

    Q_SIGNALS:
        void serviceInitialized(const QString &name);

    private:
        class Private;
        Private * d;
    };

}

#endif // NEPOMUK_TESTBASE_H
