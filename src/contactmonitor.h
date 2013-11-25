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

#ifndef CONTACTMONITOR_H
#define CONTACTMONITOR_H

#include <QObject>

#include "kpeople_export.h"

#include <KABC/Addressee>

class KPEOPLE_EXPORT ContactMonitor: public QObject
{
Q_OBJECT
public:
    ContactMonitor(const QString &contactId);
    virtual ~ContactMonitor();
    QString contactId() const;
    KABC::Addressee contact() const;
Q_SIGNALS:
    void contactChanged();
protected:
    void setContact(const KABC::Addressee &contact);
private:
    QString m_contactId;
    KABC::Addressee m_contact;
};

typedef QSharedPointer<ContactMonitor> ContactMonitorPtr;

#endif // CONTACTMONITOR_H
