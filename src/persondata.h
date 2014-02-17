/*
    KPeople
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

#ifndef PERSONDATA_H
#define PERSONDATA_H

#include "kpeople_export.h"

#include <QObject>
#include <KABC/Addressee>

#include "global.h"

namespace KPeople
{
class PersonDataPrivate;

class PersonData;

class KPEOPLE_EXPORT PersonData : public QObject
{
    Q_OBJECT

    public:
        /** Creates a Person object from a given ID.
         * The ID can be either a local application specific ID (such as akonadi://?item=15)
         * or a kpeople ID in the form kpeople://15
         */
        PersonData(const QString &id, QObject *parent=0);

        virtual ~PersonData();

        /**
         * Returns the aggregated contact information from all sources
         */
        KABC::Addressee person() const;

        /**
         * Returns information from each contact source
         */
        KABC::AddresseeList contacts() const;

    Q_SIGNALS:
        /**
         * One of the contact sources has changed
         */
        void dataChanged();

    private Q_SLOTS:
        void onContactChanged();

    private:
        Q_DISABLE_COPY(PersonData)
        Q_DECLARE_PRIVATE(PersonData)
        PersonDataPrivate * d_ptr;
};
}

#endif // PERSONDATA_H
