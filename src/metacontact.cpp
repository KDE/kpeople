/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  David Edmundson <d.edmundson@lboro.ac.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "metacontact.h"
#include <QSharedData>

namespace KPeople {
class MetaContactData : public QSharedData
{
public:
    QString personId;
    KABC::Addressee::Map contacts;
    KABC::Addressee personAddressee;
};
}

using namespace KPeople;

MetaContact::MetaContact():
d(new MetaContactData)
{

}

MetaContact::MetaContact(const QString& personId, const KABC::Addressee::Map& contacts):
d (new MetaContactData)
{
    d->personId = personId;
    d->contacts = contacts;
    reload();
}

MetaContact::MetaContact(const QString& contactId, const KABC::Addressee contact):
d (new MetaContactData)
{
    d->personId = contactId;
    d->contacts[contactId] = contact;
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

KABC::Addressee MetaContact::contact(const QString& contactId)
{
    return d->contacts[contactId];
}

KABC::AddresseeList MetaContact::contacts() const
{
    return d->contacts.values();
}

KABC::Addressee MetaContact::personAddressee() const
{
    return d->personAddressee;
}

void MetaContact::updateContact(const QString& contactId, const KABC::Addressee& contact)
{
    d->contacts[contactId] = contact;
    reload();
}

void MetaContact::removeContact(const QString& contactId)
{
    d->contacts.remove(contactId);
    reload();
}

void MetaContact::reload()
{
    //always favour the first item

    //TODO - long term goal: resource priority - local vcards for "people" trumps anything else. So we can set a preferred name etc.

    //Optimisation, if only one contact use that for everything
    if (d->contacts.size() == 1) {
        d->personAddressee = d->contacts.values().first();
        return;
    }

    d->personAddressee = KABC::Addressee();

    Q_FOREACH(const KABC::Addressee &contact, d->contacts.values()) {
        //set items with multiple cardinality
        Q_FOREACH(const KABC::Address &address, contact.addresses()) {
            d->personAddressee.insertAddress(address);
        }
        Q_FOREACH(const QString &category, contact.categories()) {
            d->personAddressee.insertCategory(category);
        }
        Q_FOREACH(const QString &email, contact.emails()) {
            d->personAddressee.insertEmail(email);
        }
        Q_FOREACH(const KABC::Key &key, contact.keys()) {
            d->personAddressee.insertKey(key);
        }
        Q_FOREACH(const KABC::PhoneNumber &phoneNumber, contact.phoneNumbers()) {
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

        if (!d->personAddressee.geo().isValid() && contact.timeZone().isValid()) {
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

        QStringList tmp = d->personAddressee.customs();
        d->personAddressee.setCustoms(tmp << contact.customs());

//         void setSound( const Sound &sound );

        //TODO something clever here
//         void setCustoms( const QStringList & );
    }
}
