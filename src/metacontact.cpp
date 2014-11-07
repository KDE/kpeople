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
#include <KContacts/Addressee>

namespace KPeople {
class MetaContactData : public QSharedData
{
public:
    QString personId;
    QStringList contactIds;
    KContacts::AddresseeList contacts; //TODO vector
    KContacts::Addressee personAddressee;
};
}

using namespace KPeople;

MetaContact::MetaContact():
d(new MetaContactData)
{

}

MetaContact::MetaContact(const QString& personId, const KContacts::Addressee::Map& contacts):
d (new MetaContactData)
{
    d->personId = personId;

    KContacts::Addressee::Map::const_iterator it = contacts.constBegin();
    while (it != contacts.constEnd()) {
        insertContactInternal(it.key(), it.value());
        it++;
    }
    reload();
}

MetaContact::MetaContact(const QString &contactId, const KContacts::Addressee &contact):
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

KContacts::Addressee MetaContact::contact(const QString& contactId)
{
    int index = d->contactIds.indexOf(contactId);
    if (index >= 0) {
        return d->contacts[index];
    } else {
        return KContacts::Addressee();
    }
}

KContacts::AddresseeList MetaContact::contacts() const
{
    return d->contacts;
}

const KContacts::Addressee& MetaContact::personAddressee() const
{
    return d->personAddressee;
}

int MetaContact::insertContact(const QString &contactId, const KContacts::Addressee &contact)
{
    int index = insertContactInternal(contactId, contact);
    reload();
    return index;
}


int MetaContact::insertContactInternal(const QString &contactId, const KContacts::Addressee &contact)
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

int MetaContact::updateContact(const QString& contactId, const KContacts::Addressee& contact)
{
    const int index = d->contactIds.indexOf(contactId);
    if (index >= 0) {
        d->contacts[index] = contact;
    }
    reload();
    return index;
}

int MetaContact::removeContact(const QString& contactId)
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

    //TODO - long term goal: resource priority - local vcards for "people" trumps anything else. So we can set a preferred name etc.

    //Optimization, if only one contact use that for everything
    if (d->contacts.size() == 1) {
        d->personAddressee = d->contacts.first();
        return;
    }

    d->personAddressee = KContacts::Addressee();

    Q_FOREACH(const KContacts::Addressee &contact, d->contacts) {
        //set items with multiple cardinality
        Q_FOREACH(const KContacts::Address &address, contact.addresses()) {
            d->personAddressee.insertAddress(address);
        }
        Q_FOREACH(const QString &category, contact.categories()) {
            d->personAddressee.insertCategory(category);
        }
        Q_FOREACH(const QString &email, contact.emails()) {
            d->personAddressee.insertEmail(email);
        }
        Q_FOREACH(const KContacts::Key &key, contact.keys()) {
            d->personAddressee.insertKey(key);
        }
        Q_FOREACH(const KContacts::PhoneNumber &phoneNumber, contact.phoneNumbers()) {
            d->personAddressee.insertPhoneNumber(phoneNumber);
        }
        //TODO customs

        //set items with single cardinality
        if (d->personAddressee.name().isEmpty() && !contact.name().isEmpty()) {
            d->personAddressee.setName(contact.name());
        }

        if (d->personAddressee.formattedName().isEmpty() && !contact.formattedName().isEmpty()) {
            d->personAddressee.setFormattedName(contact.formattedName());
        }

        //TODO all the remaining items below.

        //Maybe we can use a macro?
        if (d->personAddressee.familyName().isEmpty() && !contact.familyName().isEmpty()) {
            d->personAddressee.setFamilyName(contact.familyName());
        }

        if (d->personAddressee.givenName().isEmpty() && !contact.givenName().isEmpty()) {
            d->personAddressee.setGivenName(contact.givenName());
        }

        if (d->personAddressee.additionalName().isEmpty() && !contact.additionalName().isEmpty()) {
            d->personAddressee.setAdditionalName(contact.additionalName());
        }

        if (d->personAddressee.prefix().isEmpty() && !contact.prefix().isEmpty()) {
            d->personAddressee.setPrefix(contact.prefix());
        }

        if (d->personAddressee.suffix().isEmpty() && !contact.suffix().isEmpty()) {
            d->personAddressee.setSuffix(contact.suffix());
        }

        if (d->personAddressee.nickName().isEmpty() && !contact.nickName().isEmpty()) {
            d->personAddressee.setNickName(contact.nickName());
        }

//TODO merge Mck18's magic code that mixes years and dates
//         void setBirthday( const QDateTime &birthday );

        if (d->personAddressee.birthday().isNull() && !contact.birthday().isNull()) {
            d->personAddressee.setBirthday(contact.birthday());
        }

        if (d->personAddressee.mailer().isEmpty() && !contact.mailer().isEmpty()) {
            d->personAddressee.setMailer(contact.mailer());
        }

        if (!d->personAddressee.timeZone().isValid() && contact.timeZone().isValid()) {
            d->personAddressee.setTimeZone(contact.timeZone());
        }

        if (!d->personAddressee.geo().isValid() && contact.geo().isValid()) {
            d->personAddressee.setGeo(contact.geo());
        }

        if (d->personAddressee.title().isEmpty() && !contact.title().isEmpty()) {
            d->personAddressee.setTitle(contact.title());
        }

        if (d->personAddressee.role().isEmpty() && !contact.role().isEmpty()) {
            d->personAddressee.setRole(contact.role());
        }

        if (d->personAddressee.organization().isEmpty() && !contact.organization().isEmpty()) {
            d->personAddressee.setOrganization(contact.organization());
        }

        if (d->personAddressee.department().isEmpty() && !contact.department().isEmpty()) {
            d->personAddressee.setDepartment(contact.department());
        }


//         void setNote( const QString &note );

//         void setProductId( const QString &productId );

//         don't handle revision - it's useless in this context
//         don't handle URL - it's not for websites, it's for a remote ID


        if (!d->personAddressee.secrecy().isValid() && !contact.secrecy().isValid()) {
            d->personAddressee.setSecrecy(contact.secrecy());
        }

        if (d->personAddressee.photo().isEmpty() && !contact.photo().isEmpty()) {
            d->personAddressee.setPhoto(contact.photo());
        }

        // find most online presence
        const QString &contactPresence = contact.custom(QStringLiteral("telepathy"), QStringLiteral("presence"));
        const QString &currentPersonPresence = d->personAddressee.custom(QStringLiteral("telepathy"), QStringLiteral("presence"));

        // FIXME This needs to be redone when presence changes
        if (!contactPresence.isEmpty()) {
            if (KPeople::presenceSortPriority(contactPresence) < KPeople::presenceSortPriority(currentPersonPresence)) {
                d->personAddressee.insertCustom(QStringLiteral("telepathy"), QStringLiteral("presence"), contactPresence);
                d->personAddressee.insertCustom(QStringLiteral("telepathy"), QStringLiteral("contactId"), contact.custom(QStringLiteral("telepathy"), QStringLiteral("contactId")));
                d->personAddressee.insertCustom(QStringLiteral("telepathy"), QStringLiteral("accountPath"), contact.custom(QStringLiteral("telepathy"), QStringLiteral("accountPath")));
            }
        }

//         void setSound( const Sound &sound );

        //TODO something clever here
//         void setCustoms( const QStringList & );
    }
}
