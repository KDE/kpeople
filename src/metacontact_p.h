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
#include "backends/abstractcontact.h"

#include <kpeople/kpeople_export.h>

namespace KPeople
{
class MetaContactData;

class MetaContact
{
public:
    MetaContact();

    /** Create a 'MetaContact' from a single contact*/
    MetaContact(const QString &contactUri, const AbstractContact::Ptr &contact);

    /** Create a MetaContact with a given person ID and a map of all associated contacts*/
    MetaContact(const QString &personUri, const QMap<QString, AbstractContact::Ptr> &contacts);
    MetaContact(const MetaContact &other);
    ~MetaContact();

    MetaContact &operator=(const MetaContact &other);

    QString id() const;
    bool isValid() const;

    QStringList contactUris() const;
    AbstractContact::List contacts() const;

    AbstractContact::Ptr contact(const QString &contactUri);
    const AbstractContact::Ptr &personAddressee() const;

    //update one of the stored contacts in this metacontact object
    //@return the index of the contact which was inserted

    int insertContact(const QString &contactUri, const AbstractContact::Ptr &contact);

    int updateContact(const QString &contactUri, const AbstractContact::Ptr &contact);

    int removeContact(const QString &contactUri);

private:
    //does the real inserting contacts. Split so that we don't call the expensive "reload" function
    //multiple times at startup
    int insertContactInternal(const QString &contactUri, const AbstractContact::Ptr &contact);

    void reload();

    QSharedDataPointer<MetaContactData> d;
};
}

Q_DECLARE_TYPEINFO(KPeople::MetaContact, Q_MOVABLE_TYPE);

#endif // METACONTACT_H
