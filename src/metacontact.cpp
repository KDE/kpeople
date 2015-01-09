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

namespace KPeople {
class MetaContactData : public QSharedData
{
public:
    QString personId;
    QStringList contactIds;
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
            const QString prop = key.mid(4);
            Q_FOREACH (const AbstractContact::Ptr &contact, m_contacts) {
                QVariant val = contact->customProperty(prop);
                Q_ASSERT(val.canConvert<QVariantList>() || val.isNull());

                if (!val.isNull())
                    ret.append(val.toList());
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

MetaContact::MetaContact(const QString &personId, const QMap<QString, AbstractContact::Ptr>& contacts):
d (new MetaContactData)
{
    d->personId = personId;

    QMap<QString, AbstractContact::Ptr>::const_iterator it = contacts.constBegin();
    while (it != contacts.constEnd()) {
        insertContactInternal(it.key(), it.value());
        it++;
    }
    reload();
}

MetaContact::MetaContact(const QString &contactId, const AbstractContact::Ptr &contact):
d (new MetaContactData)
{
    d->personId = contactId;
    insertContactInternal(contactId, contact);
    reload();
}


MetaContact::MetaContact(const MetaContact &other)
:d (other.d)
{

}

MetaContact& MetaContact::operator=( const MetaContact &other )
{
  if ( this != &other )
    d = other.d;

  return *this;
}

MetaContact::~MetaContact()
{

}

QString MetaContact::id() const
{
    return d->personId;
}

bool MetaContact::isValid() const
{
    return !d->contacts.isEmpty();
}

QStringList MetaContact::contactIds() const
{
    return d->contactIds;
}

AbstractContact::Ptr MetaContact::contact(const QString &contactId)
{
    int index = d->contactIds.indexOf(contactId);
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

const AbstractContact::Ptr& MetaContact::personAddressee() const
{
    return d->personAddressee;
}

int MetaContact::insertContact(const QString &contactId, const AbstractContact::Ptr &contact)
{
    int index = insertContactInternal(contactId, contact);
    reload();
    return index;
}


int MetaContact::insertContactInternal(const QString &contactId, const AbstractContact::Ptr &contact)
{
    if (d->contactIds.contains(contactId)) {
        //if item is already listed, do nothing.
        return -1;
    } else {
        //TODO if from the local address book - prepend to give higher priority.
        int index = d->contacts.size();
        d->contacts.append(contact);
        d->contactIds.append(contactId);
        return index;
    }
}

int MetaContact::updateContact(const QString &contactId, const AbstractContact::Ptr& contact)
{
    const int index = d->contactIds.indexOf(contactId);
    if (index >= 0) {
        d->contacts[index] = contact;
    }
    reload();
    return index;
}

int MetaContact::removeContact(const QString &contactId)
{
    const int index = d->contactIds.indexOf(contactId);
    if (index >= 0) {
        d->contacts.removeAt(index);
        d->contactIds.removeAt(index);
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
