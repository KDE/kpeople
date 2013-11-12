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
#include <QUrl>
#include <QStringList>
#include <KDateTime>
#include <KABC/Addressee>

namespace KPeople
{
struct PersonDataPrivate;

class PersonData;

class KPEOPLE_EXPORT PersonData : public QObject
{
    Q_OBJECT

    public:
        PersonData(const QString &contactId, QObject *parent=0);
//         PersonData(const MetaContact &mc, QObject *parent=0);

        virtual ~PersonData();
        KABC::Addressee person() const;

    Q_SIGNALS:
        /** Some of the person's data we're offering has changed */
        void dataChanged();

    protected:

    private:
        Q_DISABLE_COPY(PersonData)
        Q_DECLARE_PRIVATE(PersonData)
        PersonDataPrivate * d_ptr;
};
}

#endif // PERSONDATA_H
