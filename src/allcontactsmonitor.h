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

#ifndef ALLCONTACTSMONITOR_H
#define ALLCONTACTSMONITOR_H

#include <QObject>

#include "kpeople_export.h"

#include <KABC/Addressee>

namespace KPeople {

class AllContactsMonitorPrivate;

/**
 * This class should be subclassed by each datasource and return a list of
 * all contacts that the datasource knows about.
 *
 * Subclasses are expected to be asynchronous
 *
 */
class KPEOPLE_EXPORT AllContactsMonitor : public QObject
{
    Q_OBJECT
public:
    explicit AllContactsMonitor(); //TODO make protected? this isn't useful unless subclassed
    virtual ~AllContactsMonitor();

    /**
     * Returns all currently loaded contacts
     */
    virtual KABC::Addressee::Map contacts();

    //TODO redo as a state enum - InitialLoad, Fail, Loaded
    bool isInitialFetchComplete() const;
    
    bool initialFetchSuccess() const;

Q_SIGNALS:
    /**
     * DataSources should emit this whenever a known contact changes
     */
    void contactChanged(const QString &contactId, const KABC::Addressee &contact);

    /**
     * DataSources should emit this whenever a contact is added
     */
    void contactAdded(const QString &contactId, const KABC::Addressee &contact);

    /**
     * DataSources should emit this whenever a contact is removed and they are no longer able to supply up-to-date data on a contact
     */
    void contactRemoved(const QString &contactId);

    /**
     * Notifies that the DataSource has completed it's initial fetch.
     *
     * @warning DataSources should use emitInitialFetchComplete() instead of emitting this signal
     * directly.
     *
     * @param success True when the fetch was successful, False when an error occurred.
     */
    void initialFetchComplete( bool success );

protected Q_SLOTS:
    /**
     * DataSources should call this once they have finished initial retrieval of all contacts from their
     * storage.
     *
     * This will emit initialFetchComplete() signal
     *
     * @p success Whether the fetch was successful.
     */
    void emitInitialFetchComplete( bool success );

private:
    Q_DISABLE_COPY(AllContactsMonitor)
    Q_DECLARE_PRIVATE(AllContactsMonitor)
    AllContactsMonitorPrivate *d_ptr;
};

}
typedef QSharedPointer<KPeople::AllContactsMonitor> AllContactsMonitorPtr;

#endif // ALLCONTACTSMONITOR_H
