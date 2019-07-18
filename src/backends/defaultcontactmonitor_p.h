/*
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

#ifndef DEFAULTCONTACTMONITOR_H
#define DEFAULTCONTACTMONITOR_H

#include <QSharedPointer>

#include "backends/contactmonitor.h"
#include "backends/allcontactsmonitor.h"

using namespace KPeople;

/*
 * If plugins don't implement a ContactWatcher, we repurpose the whole model, and single out changes for one contact
 * ideally plugins (especially slow ones) will implement their own contact monitor.
*/

class KPEOPLEBACKEND_EXPORT DefaultContactMonitor : public ContactMonitor
{
    Q_OBJECT
public:
    DefaultContactMonitor(const QString &contactUri, const AllContactsMonitorPtr &allContactsWatcher);
private Q_SLOTS:
    void onContactAdded(const QString &contactUri, const AbstractContact::Ptr &contact);
    void onContactChanged(const QString &contactUri, const AbstractContact::Ptr &contact);
    void onContactRemoved(const QString &contactUri);
private:
    AllContactsMonitorPtr m_allContactsMonitor;
};

#endif // DEFAULTCONTACTMONITOR_H
