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

#include "contactmonitor.h"

using namespace KPeople;

class KPeople::ContactMonitorPrivate
{
public:
    QString m_contactId;
    KABC::Addressee m_contact;
};



ContactMonitor::ContactMonitor(const QString &contactId)
    : QObject(0),
      d_ptr(new ContactMonitorPrivate)
{
    Q_D(ContactMonitor);
    d->m_contactId = contactId;
}

ContactMonitor::~ContactMonitor()
{
    delete d_ptr;
}

void ContactMonitor::setContact(const KABC::Addressee &contact)
{
    Q_D(ContactMonitor);

    d->m_contact = contact;
    Q_EMIT contactChanged();
}

KABC::Addressee ContactMonitor::contact() const
{
    Q_D(const ContactMonitor);

    return d->m_contact;
}

QString ContactMonitor::contactId() const
{
    Q_D(const ContactMonitor);

    return d->m_contactId;
}


#include "contactmonitor.moc"
