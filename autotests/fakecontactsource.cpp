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

KPeople::AllContactsMonitor *FakeContactSource::createAllContactsMonitor()
{
    return new FakeAllContactsMonitor();
}

void FakeContactSource::changeProperty(const QString& key, const QVariant& value)
{
    qobject_cast<FakeAllContactsMonitor *>(allContactsMonitor().data())->changeProperty(key, value);
}

//----------------------------------------------------------------------------

class FakeContact : public KPeople::AbstractContact
{
public:
    FakeContact(const QVariantMap &props)
        : m_properties(props)
    {}

    virtual QVariant customProperty(const QString &key) const
    {
        if (key.startsWith(QLatin1Literal("all-"))) {
            return QStringList(m_properties[key.mid(4)].toString());
        } else {
            return m_properties[key];
        }
    }

    QVariantMap m_properties;
};

FakeAllContactsMonitor::FakeAllContactsMonitor()
{
}

QMap<QString, KPeople::AbstractContact::Ptr> FakeAllContactsMonitor::contacts()
{
    static QMap<QString, KPeople::AbstractContact::Ptr> contacts;

    if (contacts.isEmpty()) {
        {
            KPeople::AbstractContact::Ptr contact1(new FakeContact(QVariantMap {
                { KPeople::AbstractContact::NameProperty, QStringLiteral("Contact 1") },
                { KPeople::AbstractContact::EmailProperty, QStringLiteral("contact1@example.com") }
            }));
            contacts[QStringLiteral("fakesource://contact1")] = contact1;
        }

        {
            KPeople::AbstractContact::Ptr contact2(new FakeContact(QVariantMap {
                { KPeople::AbstractContact::NameProperty, QStringLiteral("Person A") },
                { KPeople::AbstractContact::EmailProperty, QStringLiteral("contact2@example.com") }
            }));
            contacts[QStringLiteral("fakesource://contact2")] = contact2;
        }

        {
            KPeople::AbstractContact::Ptr contact3(new FakeContact(QVariantMap {
                { KPeople::AbstractContact::NameProperty, QStringLiteral("Person A") },
                { KPeople::AbstractContact::EmailProperty, QStringLiteral("contact3@example.com") }
            }));
            contacts[QStringLiteral("fakesource://contact3")] = contact3;
        }
    }

    return contacts;
}

void FakeAllContactsMonitor::changeProperty(const QString& key, const QVariant& value)
{
    KPeople::AbstractContact::Ptr contact1 = contacts()[QStringLiteral("fakesource://contact1")];
    static_cast<FakeContact *>(contact1.data())->m_properties[key] = value;
    Q_ASSERT(contact1->customProperty(key) == value);

    Q_EMIT contactChanged(QStringLiteral("fakesource://contact1"), contact1);
}

#include "fakecontactsource.moc"
