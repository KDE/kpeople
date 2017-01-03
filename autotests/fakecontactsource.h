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

#ifndef FAKECONTACTSOURCE_H
#define FAKECONTACTSOURCE_H

#include <backends/basepersonsdatasource.h>
#include <backends/allcontactsmonitor.h>

/**
the fake datasource is configured with

ID       - NAME        - email                  - phone
contact1 - Contact1    - contact1@example.com   -
contact2 - PersonA     - contact2@example.com   - +1 234 567 890
contact3 - PersonA     - contact3@example.com   - +34 666 777 999
contact4 - PersonA     - contact4@example.com
*/

class FakeContactSource : public KPeople::BasePersonsDataSource
{
public:
    FakeContactSource(QObject *parent, const QVariantList &args = QVariantList());
    QString sourcePluginId() const Q_DECL_OVERRIDE;

    void changeProperty(const QString& key, const QVariant& value);
protected:
    KPeople::AllContactsMonitor *createAllContactsMonitor() Q_DECL_OVERRIDE;
};

//----------------------------------------------------------------------------

class FakeAllContactsMonitor : public KPeople::AllContactsMonitor
{
    Q_OBJECT
public:
    explicit FakeAllContactsMonitor();
    void changeProperty(const QString& key, const QVariant& value);
    QMap<QString, KPeople::AbstractContact::Ptr> contacts() Q_DECL_OVERRIDE;
};

#endif // FAKECONTACTSOURCE_H
