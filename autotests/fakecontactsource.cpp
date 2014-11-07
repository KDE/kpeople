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
    return QStringLiteral("fakesource://");
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

KContacts::Addressee::Map FakeAllContactsMonitor::contacts()
{
    KContacts::Addressee::Map contacts;

    {
        KContacts::Addressee contact1;
        contact1.setName(QStringLiteral("Contact 1"));
        contact1.setEmails(QStringList() << QStringLiteral("contact1@example.com"));
        contacts[QStringLiteral("fakesource://contact1")] = contact1;
    }

    {
        KContacts::Addressee contact2;
        contact2.setName(QStringLiteral("Person A"));
        contact2.setEmails(QStringList() << QStringLiteral("contact2@example.com"));
        contacts[QStringLiteral("fakesource://contact2")] = contact2;
    }

    {
        KContacts::Addressee contact3;
        contact3.setName(QStringLiteral("Person A"));
        contact3.setEmails(QStringList() << QStringLiteral("contact3@example.com"));
        contacts[QStringLiteral("fakesource://contact3")] = contact3;
    }

    return contacts;
}

void FakeAllContactsMonitor::changeContact1Email()
{
    KContacts::Addressee contact1 = contacts()[QStringLiteral("fakesource://contact1")];
    contact1.setEmails(QStringList() << QStringLiteral("newaddress@yahoo.com"));

    Q_EMIT contactChanged(QStringLiteral("fakesource://contact1"), contact1);
}

#include "fakecontactsource.moc"
