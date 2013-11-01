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

class MetaContactData : public QSharedData
{
public:
    KABC::AddresseeList contacts;
    KABC::Addressee personAddressee;
};

MetaContact::MetaContact():
d(new MetaContactData)
{

}

MetaContact::MetaContact(const KABC::AddresseeList &contacts):
d (new MetaContactData)
{
    updateContacts(contacts);
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

// MetaContact& MetaContact::operator=(const MetaContact& other)
// {
//
// }

KABC::AddresseeList MetaContact::contacts() const
{
    return d->contacts;
}

KABC::Addressee MetaContact::personAddressee() const
{
    return d->personAddressee;
}

void MetaContact::updateContacts(const KABC::AddresseeList& contacts)
{
    if (d->contacts == contacts) {
        return;
    }

    d->contacts = contacts;
    //reset person vcard, then populate it with all the data we have available
    //always favour the first item

    //TODO - long term goal: resource priority - local vcards for "people" trumps anything else. So we can set a preferred name etc.

    //TODO - optimisation, if only one contact, simply set d->personAddressee to contacts.first() ?

    d->personAddressee = KABC::Addressee();

    Q_FOREACH(const KABC::Addressee &contact, d->contacts) {
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


//         void setMailer( const QString &mailer );
//         void setTimeZone( const TimeZone &timeZone );
//         void setGeo( const Geo &geo );
//         void setTitle( const QString &title );
//         void setRole( const QString &role );
//         void setOrganization( const QString &organization );
//         void setDepartment( const QString &department );
//         void setNote( const QString &note );

//         void setProductId( const QString &productId );
//         void setRevision( const QDateTime &revision );
//         void setSortString( const QString &sortString );
//         void setUrl( const KUrl &url );
//         void setSecrecy( const Secrecy &secrecy );
//         void setLogo( const Picture &logo );
//         void setPhoto( const Picture &photo );

        if (d->personAddressee.photo().isEmpty() && !contact.photo().isEmpty()) {
            d->personAddressee.setPhoto(contact.photo());
        }

//         void setSound( const Sound &sound );
//except maybe not this
//         void setCustoms( const QStringList & );
    }
}
