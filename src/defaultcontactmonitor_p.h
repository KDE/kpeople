/*
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

#ifndef DEFAULTCONTACTMONITOR_H
#define DEFAULTCONTACTMONITOR_H

#include <QObject>

#include "contactmonitor.h"
#include "allcontactsmonitor.h"

using namespace KPeople;

/*
 * If plugins don't implement a ContactWatcher, we repurpose the whole model, and single out changes for one contact
 * ideally plugins (especially slow ones) will implement their own contact monitor.
*/

class DefaultContactMonitor : public ContactMonitor
{
    Q_OBJECT
public:
    DefaultContactMonitor(const QString &contactId, const AllContactsMonitorPtr &allContactsWatcher);
private Q_SLOTS:
    void onContactAdded(const QString &contactId, const KABC::Addressee &contact);
    void onContactChanged(const QString &contactId, const KABC::Addressee &contact);
    void onContactRemoved(const QString &contactId);
private:
    AllContactsMonitorPtr m_allContactsMonitor;
};

#endif // DEFAULTCONTACTMONITOR_H
