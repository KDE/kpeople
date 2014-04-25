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

#include "allcontactsmonitor.h"

#include <KABC/Addressee>

using namespace KPeople;

class KPeople::AllContactsMonitorPrivate
{
  public:
    AllContactsMonitorPrivate():
        m_initialFetchDone(false),
        m_initialFetchSucccess(false)
    {
    }

    bool m_initialFetchDone;
    bool m_initialFetchSucccess;
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

KABC::Addressee::Map AllContactsMonitor::contacts()
{
    return KABC::Addressee::Map();
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



#include "allcontactsmonitor.moc"
