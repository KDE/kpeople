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

#ifndef KPEOPLE_CONTACT_H
#define KPEOPLE_CONTACT_H

#include <KABC/Addressee>

namespace KPeople {

class ContactPrivate;

class Contact : public KABC::Addressee
{
public:
    Contact(const QString &uri, const KABC::Addressee &address);
    Contact(const Contact &other);

    Contact &operator=(const Contact &other);

    QString uri() const;

    //FUTURE
    //presence();
    //bool isCustom();

private:
    QSharedDataPointer<ContactPrivate> d;
};

}

#endif // CONTACT_H
