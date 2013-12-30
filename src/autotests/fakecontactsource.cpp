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

#include "fakecontactsource.h"

FakeContactSource::FakeContactSource(QObject *parent, const QVariantList &args)
    : BasePersonsDataSource(parent, args)
{
}

QString FakeContactSource::sourcePluginId() const
{
    return "fakesource://";
}

KPeople::AllContactsMonitor* FakeContactSource::createAllContactsMonitor()
{
    return new FakeAllContactsMonitor();
}


void FakeContactSource::changeContact1Email()
{
    qobject_cast<FakeAllContactsMonitor*>(allContactsMonitor().data())->changeContact1Email();
}


//----------------------------------------------------------------------------


FakeAllContactsMonitor::FakeAllContactsMonitor()
{
}

KABC::Addressee::Map FakeAllContactsMonitor::contacts()
{
    KABC::Addressee::Map contacts;

    {
        KABC::Addressee contact1;
        contact1.setName("Contact 1");
        contact1.setEmails(QStringList() << "contact1@example.com");
        contacts["fakesource://contact1"] = contact1;
    }

    {
        KABC::Addressee contact2;
        contact2.setName("Person A");
        contact2.setEmails(QStringList() << "contact2@example.com");
        contacts["fakesource://contact2"] = contact2;
    }

    {
        KABC::Addressee contact3;
        contact3.setName("Person A");
        contact3.setEmails(QStringList() << "contact3@example.com");
        contacts["fakesource://contact3"] = contact3;
    }

    return contacts;
}

void FakeAllContactsMonitor::changeContact1Email()
{
    KABC::Addressee contact1 = contacts()["fakesource://contact1"];
    contact1.setEmails(QStringList() << "newaddress@yahoo.com");

    Q_EMIT contactChanged("fakesource://contact1", contact1);
}

#include "fakecontactsource.moc"
