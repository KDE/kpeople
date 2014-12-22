/*
    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "basepersonsdatasource.h"

#include <QDebug>


#include "defaultcontactmonitor_p.h"

using namespace KPeople;

class KPeople::BasePersonsDataSourcePrivate
{
public:
    QWeakPointer<AllContactsMonitor> m_allContactsMonitor;
    QHash<QString, QWeakPointer<ContactMonitor> > m_contactMonitors;
};


BasePersonsDataSource::BasePersonsDataSource(QObject *parent, const QVariantList &args)
    : QObject(parent),
      d_ptr(new BasePersonsDataSourcePrivate)
{
    Q_UNUSED(args)
}

BasePersonsDataSource::~BasePersonsDataSource()
{
    delete d_ptr;
}

AllContactsMonitorPtr BasePersonsDataSource::allContactsMonitor()
{
    Q_D(BasePersonsDataSource);

    //if there is currently no watcher, create one
    AllContactsMonitorPtr c;
    if (!d->m_allContactsMonitor.toStrongRef()) {
        c = AllContactsMonitorPtr(createAllContactsMonitor());
        d->m_allContactsMonitor = c;
    }

    return d->m_allContactsMonitor.toStrongRef();
}

ContactMonitorPtr BasePersonsDataSource::contactMonitor(const QString& contactId)
{
    Q_D(BasePersonsDataSource);

    ContactMonitorPtr c;
    if (!d->m_contactMonitors[contactId].toStrongRef()) {
        c = ContactMonitorPtr(createContactMonitor(contactId));
        d->m_contactMonitors[contactId] = c;
    }
    return d->m_contactMonitors[contactId].toStrongRef();
}

ContactMonitor* BasePersonsDataSource::createContactMonitor(const QString &contactId)
{
    return new DefaultContactMonitor(contactId, allContactsMonitor());
}
