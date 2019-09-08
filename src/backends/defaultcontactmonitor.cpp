/*
    Abstract class to load a monitor changes for a single contact
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

#include "defaultcontactmonitor_p.h"

DefaultContactMonitor::DefaultContactMonitor(const QString &contactUri, const AllContactsMonitorPtr &allContactsWatcher):
    ContactMonitor(contactUri),
    m_allContactsMonitor(allContactsWatcher)
{
    connect(allContactsWatcher.data(), &AllContactsMonitor::contactAdded, this, &DefaultContactMonitor::onContactAdded);
    connect(allContactsWatcher.data(), &AllContactsMonitor::contactRemoved, this, &DefaultContactMonitor::onContactRemoved);
    connect(allContactsWatcher.data(), &AllContactsMonitor::contactChanged, this, &DefaultContactMonitor::onContactChanged);

    const QMap<QString, AbstractContact::Ptr> &contacts = m_allContactsMonitor->contacts();
    QMap<QString, AbstractContact::Ptr>::const_iterator it = contacts.constFind(contactUri);
    if (it != contacts.constEnd()) {
        setContact(it.value());
    }
}

void DefaultContactMonitor::onContactAdded(const QString &id, const AbstractContact::Ptr &contact)
{
    if (id == contactUri()) {
        setContact(contact);
    }
}

void DefaultContactMonitor::onContactChanged(const QString &id, const AbstractContact::Ptr &contact)
{
    if (id == contactUri()) {
        setContact(contact);
    }
}

void DefaultContactMonitor::onContactRemoved(const QString &id)
{
    if (id == contactUri()) {
        setContact(AbstractContact::Ptr());
    }
}

