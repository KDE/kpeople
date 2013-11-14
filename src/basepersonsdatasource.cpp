/*
    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>

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

using namespace KPeople;

#include "defaultcontactmonitor_p.h"

//TODO d pointer this

BasePersonsDataSource::BasePersonsDataSource(QObject *parent, const QVariantList &args)
    : QObject(parent)
{
    Q_UNUSED(args)
}

BasePersonsDataSource::~BasePersonsDataSource()
{

}

AllContactsMonitorPtr BasePersonsDataSource::allContactsMonitor()
{
    //if there is currently no watcher, create one
    AllContactsMonitorPtr c;
    if (!m_allContactsMonitor.toStrongRef()) {
        c = AllContactsMonitorPtr(createAllContactsMonitor());
        m_allContactsMonitor = c;
    }

    return m_allContactsMonitor.toStrongRef();
}

ContactMonitorPtr BasePersonsDataSource::contactMonitor(const QString& contactId)
{
    ContactMonitorPtr c;
    if (!m_contactMonitors[contactId].toStrongRef()) {
        c = ContactMonitorPtr(createContactMonitor(contactId));
        m_contactMonitors[contactId] = c;
    }
    return m_contactMonitors[contactId].toStrongRef();
}

ContactMonitor* BasePersonsDataSource::createContactMonitor(const QString &contactId)
{
    return new DefaultContactMonitor(contactId, m_allContactsMonitor);
}
