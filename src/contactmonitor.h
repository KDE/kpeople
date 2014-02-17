/*
 * Abstract class to load a monitor changes for a single contact
 * Copyright 2013  David Edmundson <davidedmundson@kde.org>
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

namespace KPeople {

class ContactMonitorPrivate;

/**
 * This class loads data for a single contact from a datasource.
 *
 * Datasources should subclass this and call setContact() when the contact loads or changes.
 * It is used for optimising performance over loading all contacts and filtering the results.
 * Subclasses are expected to be asynchronous in loading data.
 *
 */
class KPEOPLE_EXPORT ContactMonitor: public QObject
{
Q_OBJECT
public:
    ContactMonitor(const QString &contactId);
    virtual ~ContactMonitor();

    /**
     * The ID of the contact being loaded
     */
    QString contactId() const;

    /**
     * The currently loaded information on this contact.
     */
    KABC::Addressee contact() const;
Q_SIGNALS:
    /**
     * Emitted whenever the contact changes
     */
    void contactChanged();
protected:
    /**
     * Sets or updates the contact and emits contactChanged
     * Subclasses should call this when data is loaded or changes
     */
    void setContact(const KABC::Addressee &contact);
private:
    Q_DISABLE_COPY(ContactMonitor)
    Q_DECLARE_PRIVATE(ContactMonitor)
    ContactMonitorPrivate *d_ptr;
};

}
typedef QSharedPointer<KPeople::ContactMonitor> ContactMonitorPtr;

#endif // CONTACTMONITOR_H
