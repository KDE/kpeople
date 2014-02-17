/*
 *    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>
 *    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef BASE_PERSONS_DATA_SOURCE_H
#define BASE_PERSONS_DATA_SOURCE_H

#include <QObject>
#include <QVariant>
#include <KABC/Addressee>

#include "kpeople_export.h"

#include "allcontactsmonitor.h"
#include "contactmonitor.h"

namespace KPeople
{

class BasePersonsDataSourcePrivate;

//This is a QObject for KPluginFactory
class KPEOPLE_EXPORT BasePersonsDataSource : public QObject
{
    Q_OBJECT
public:
    BasePersonsDataSource(QObject *parent, const QVariantList &args = QVariantList());
    virtual ~BasePersonsDataSource();

    /**
     * Return a ref counted AllContactMonitor which lists and monitors all contacts from a source
     */
    AllContactsMonitorPtr   allContactsMonitor();

    /**
     * Return a ref counted watcher for a single contact
     */
    ContactMonitorPtr      contactMonitor(const QString &contactId);

    /**
     * Returns the ID used by this datasource.
     * i.e if the contactIDs are in the form akonadi://?item=324 this method should return "akonadi"
     */
    virtual QString sourcePluginId() const = 0;

protected:
    virtual AllContactsMonitor* createAllContactsMonitor() = 0;

    /**
     * Base classes can implement this in order to not load every contact
     * otherwise the AllContactWatcher will be used and filtered.
     */
    virtual ContactMonitor* createContactMonitor(const QString &contactId);
private:
    Q_DISABLE_COPY(BasePersonsDataSource)
    Q_DECLARE_PRIVATE(BasePersonsDataSource)
    BasePersonsDataSourcePrivate * d_ptr;
};

}
#endif // BASE_PERSONS_DATA_SOURCE_H
