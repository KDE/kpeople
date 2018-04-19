/*
 * Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "allcontactsmonitor.h"

using namespace KPeople;

class KPeople::AllContactsMonitorPrivate
{
public:
    AllContactsMonitorPrivate()
    {
    }

    bool m_initialFetchDone = false;
    bool m_initialFetchSucccess = false;
};

AllContactsMonitor::AllContactsMonitor():
    QObject(),
    d_ptr(new AllContactsMonitorPrivate)
{

}
AllContactsMonitor::~AllContactsMonitor()
{
    delete d_ptr;
}

QMap<QString, AbstractContact::Ptr> AllContactsMonitor::contacts()
{
    return QMap<QString, AbstractContact::Ptr>();
}

bool AllContactsMonitor::isInitialFetchComplete() const
{
    return d_ptr->m_initialFetchDone;
}

bool AllContactsMonitor::initialFetchSuccess() const
{
    return d_ptr->m_initialFetchSucccess;
}

void AllContactsMonitor::emitInitialFetchComplete(bool success)
{
    d_ptr->m_initialFetchDone = true;
    d_ptr->m_initialFetchSucccess = success;
    Q_EMIT initialFetchComplete(success);
}

