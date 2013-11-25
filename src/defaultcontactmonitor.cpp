/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  David Edmundson <d.edmundson@lboro.ac.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
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
