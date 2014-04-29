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


#ifndef METACONTACT_H
#define METACONTACT_H

#include <QSharedDataPointer>


#include <KABC/Addressee>

#include "kpeople_export.h"

namespace KPeople {
class MetaContactData;

class MetaContact
{
public:
    MetaContact();
    /** Create a 'MetaContact' from a single contact*/
    MetaContact(const QString &contactId, const KABC::Addressee &contact);

    /** Create a MetaContact with a given person ID and a map of all associated contacts*/
    MetaContact(const QString &personId, const KABC::Addressee::Map& contacts);
    MetaContact(const MetaContact &other);
    ~MetaContact();

    MetaContact& operator=(const MetaContact& other);

    QString id() const;
    bool isValid() const;

    QStringList contactIds() const;
    KABC::AddresseeList contacts() const;

    KABC::Addressee contact(const QString &contactId);
    const KABC::Addressee& personAddressee() const;

    //update one of the stored contacts in this metacontact object
    //@return the index of the contact which was inserted

    int insertContact(const QString &contactId, const KABC::Addressee &contact);

    int updateContact(const QString &contactId, const KABC::Addressee &contact);

    int removeContact(const QString &contactId);

private:
    //does the real inserting contacts. Split so that we don't call the expensive "reload" function
    //multiple times at startup
    int insertContactInternal(const QString &contactId, const KABC::Addressee &contact);

    void reload();

    QSharedDataPointer<MetaContactData> d;
};
}

#endif // METACONTACT_H
