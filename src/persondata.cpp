/*
    Copyright (C) 2013  David Edmundson (davidedmundson@kde.org)

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

#include "persondata.h"

#include "datasourcewatcher_p.h"

namespace KPeople {
    class PersonDataPrivate {
        MetaContact metaContact;
    };
}

using namespace KPeople;

KPeople::PersonData::PersonData(const QString& contactId, QObject* parent):
    QObject(parent),
    d_ptr(new PersonDataPrivate)
{
    //TODO
    //query DB

    //load contacts

    //create mc

    //setup watchers

    //TODO async me
}

PersonData::~PersonData()
{
    delete d_ptr;
}

KABC::Addressee PersonData::person() const
{
    return d->metaContact.person();
}

MetaContact PersonData::metacontact() const
{
    Q_D(const PersonData);
    d->metaContact.personAddressee();
}


