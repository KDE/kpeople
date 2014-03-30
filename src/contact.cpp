/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  David Edmundson <david@davidedmundson.co.uk>
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

#include "contact.h"

using namespace KPeople;

namespace KPeople {
    class ContactPrivate : public QSharedData {
    public:
        QString kpeopleURI;

    };
}

Contact::Contact(const QString& uri, const KABC::Addressee& address):
    KABC::Addressee(address),
    d(new ContactPrivate)
{
    d->kpeopleURI = uri;
}


Contact::Contact(const Contact &other):
    KABC::Addressee(other),
    d (other.d)
{

}

Contact& Contact::operator=(const Contact &other )
{
  if ( this != &other ) {
    d = other.d;
  }

  return *this;
}

QString Contact::uri() const
{
    return d->kpeopleURI;
}
