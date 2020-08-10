/*
    SPDX-FileCopyrightText: 2013 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef AKONADIDATASOURCE_H
#define AKONADIDATASOURCE_H

#include <basepersonsdatasource.h>

#include <Akonadi/Monitor>

class AkonadiDataSource : public KPeople::BasePersonsDataSource
{
public:
    AkonadiDataSource(QObject *parent, const QVariantList &args = QVariantList());
    virtual ~AkonadiDataSource();
    virtual KPeople::AllContactsMonitor *createAllContactsMonitor();
    virtual KPeople::ContactMonitor *createContactMonitor(const QString &contactUri);
    virtual QString sourcePluginId() const;
private:
    Akonadi::Monitor *m_monitor;
};

#endif // AKONADIDATASOURCE_H
