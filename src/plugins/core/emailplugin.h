/*
    Copyright (C) 2013  David Edmundson <D.Edmundson@lboro.ac.uk>

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



#ifndef EMAIL_PLUGIN_H
#define EMAIL_PLUGIN_H

#include "abstractpersonplugin.h"

class EmailPlugin : public KPeople::AbstractPersonPlugin
{
    Q_OBJECT
public:
    EmailPlugin(QObject *parent, const QVariantList &args);

    virtual QList<QAction*> actionsForPerson(const KPeople::PersonDataPtr &personData, QObject *parent);

private Q_SLOTS:
    void onEmailTriggered();
};

#endif // EMAIL_PLUGIN_H
