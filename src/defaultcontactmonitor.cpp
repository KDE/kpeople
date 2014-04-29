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

DefaultContactMonitor::DefaultContactMonitor(const QString &contactId, const AllContactsMonitorPtr& allContactsWatcher):
    ContactMonitor(contactId),
    m_allContactsMonitor(allContactsWatcher)
{
    connect(allContactsWatcher.data(), SIGNAL(contactAdded(QString,KABC::Addressee)), SLOT(onContactAdded(QString,KABC::Addressee)));
    connect(allContactsWatcher.data(), SIGNAL(contactRemoved(QString)), SLOT(onContactRemoved(QString)));
    connect(allContactsWatcher.data(), SIGNAL(contactChanged(QString,KABC::Addressee)), SLOT(onContactChanged(QString,KABC::Addressee)));

    const KABC::Addressee::Map &contacts = m_allContactsMonitor->contacts();
    KABC::Addressee::Map::const_iterator it = contacts.constFind(contactId);
    if (it != contacts.constEnd()) {
        setContact(it.value());
    }
}

void DefaultContactMonitor::onContactAdded(const QString& id, const KABC::Addressee& contact)
{
    if (id == contactId()) {
        setContact(contact);
    }
}

void DefaultContactMonitor::onContactChanged(const QString& id, const KABC::Addressee& contact)
{
    if (id == contactId()) {
        setContact(contact);
    }
}

void DefaultContactMonitor::onContactRemoved(const QString& id)
{
    if (id == contactId()) {
        setContact(KABC::Addressee());
    }
}



#include "defaultcontactmonitor_p.moc"
