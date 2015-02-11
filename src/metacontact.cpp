/*
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

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

#include "metacontact_p.h"
#include "global.h"
#include <QSharedData>
#include <QDebug>
#include <QSet>

namespace KPeople
{
class MetaContactData : public QSharedData
{
public:
    QString personUri;
    QStringList contactUris;
    AbstractContact::List contacts;
    AbstractContact::Ptr personAddressee;
};
}

// TODO: It feels like MetaContact and MetaContactProxy should be merged,
// still, not today.

class MetaContactProxy : public KPeople::AbstractContact
{
public:
    MetaContactProxy(const AbstractContact::List &contacts)
        : m_contacts(contacts)
    {}

    virtual QVariant customProperty(const QString &key) const
    {
        if (key.startsWith(QLatin1String("all-"))) {
            QVariantList ret;
            Q_FOREACH (const AbstractContact::Ptr &contact, m_contacts) {
                QVariant val = contact->customProperty(key);
                Q_ASSERT(val.canConvert<QVariantList>() || val.isNull());

                if (!val.isNull()) {
                    ret.append(val.toList());
                }
            }
            return ret;
        } else {
            Q_FOREACH (const AbstractContact::Ptr &contact, m_contacts) {
                QVariant val = contact->customProperty(key);
                if (val.isValid()) {
                    return val;
                }
            }
            return QVariant();
        }
    }

    AbstractContact::List m_contacts;
};

using namespace KPeople;

MetaContact::MetaContact():
    d(new MetaContactData)
{
}

MetaContact::MetaContact(const QString &personUri, const QMap<QString, AbstractContact::Ptr> &contacts):
    d(new MetaContactData)
{
    d->personUri = personUri;

    QMap<QString, AbstractContact::Ptr>::const_iterator it = contacts.constBegin();
    while (it != contacts.constEnd()) {
        insertContactInternal(it.key(), it.value());
        it++;
    }
    reload();
}

MetaContact::MetaContact(const QString &contactUri, const AbstractContact::Ptr &contact):
    d(new MetaContactData)
{
    d->personUri = contactUri;
    insertContactInternal(contactUri, contact);
    reload();
}

MetaContact::MetaContact(const MetaContact &other)
    : d(other.d)
{

}

MetaContact &MetaContact::operator=(const MetaContact &other)
{
    if (this != &other) {
        d = other.d;
    }

    return *this;
}

MetaContact::~MetaContact()
{

}

QString MetaContact::id() const
{
    return d->personUri;
}

bool MetaContact::isValid() const
{
    return !d->contacts.isEmpty();
}

QStringList MetaContact::contactUris() const
{
    return d->contactUris;
}

AbstractContact::Ptr MetaContact::contact(const QString &contactUri)
{
    int index = d->contactUris.indexOf(contactUri);
    if (index >= 0) {
        return d->contacts[index];
    } else {
        return AbstractContact::Ptr();
    }
}

AbstractContact::List MetaContact::contacts() const
{
    return d->contacts;
}

const AbstractContact::Ptr &MetaContact::personAddressee() const
{
    return d->personAddressee;
}

int MetaContact::insertContact(const QString &contactUri, const AbstractContact::Ptr &contact)
{
    int index = insertContactInternal(contactUri, contact);
    if (index >= 0) {
        reload();
    } else {
        qWarning() << "Inserting an already-present contact" << contactUri;
    }
    return index;
}

int MetaContact::insertContactInternal(const QString &contactUri, const AbstractContact::Ptr &contact)
{
    if (d->contactUris.contains(contactUri)) {
        //if item is already listed, do nothing.
        return -1;
    } else {
        //TODO if from the local address book - prepend to give higher priority.
        int index = d->contacts.size();
        d->contacts.append(contact);
        d->contactUris.append(contactUri);
        return index;
    }
}

int MetaContact::updateContact(const QString &contactUri, const AbstractContact::Ptr &contact)
{
    const int index = d->contactUris.indexOf(contactUri);
    Q_ASSERT(index < 0 || d->contacts[index] == contact);
    if (index < 0) {
        qWarning() << "contact not part of the metacontact";
    }
    return index;
}

int MetaContact::removeContact(const QString &contactUri)
{
    const int index = d->contactUris.indexOf(contactUri);
    if (index >= 0) {
        d->contacts.removeAt(index);
        d->contactUris.removeAt(index);
        reload();
    }
    return index;
}

void MetaContact::reload()
{
    //always favour the first item

    //Optimization, if only one contact re-use that one
    d->personAddressee = (d->contacts.size() == 1) ? d->contacts.first() : AbstractContact::Ptr(new MetaContactProxy(d->contacts));
    Q_ASSERT(d->personAddressee);
}
