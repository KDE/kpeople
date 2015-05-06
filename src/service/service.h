/*
    D-Bus Lookup Service
    Copyright (C) 2015  Martin Klapetek <mklapetek@kde.org>

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

#ifndef KPEOPLE_SERVICE_H
#define KPEOPLE_SERVICE_H

#include <QDBusAbstractAdaptor>

namespace KPeople
{

class LookupServicePrivate;

class LookupService : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.KPeople")

public:
    LookupService(QObject *parent = 0);

public Q_SLOTS:
    /**
     * @param contact Can be either phone number or email or other contact data
     * @param hint Gives a hint what the searched contact data is, will make things faster //TODO: make it enum
     */
    QString contactIdForContactProperty(const QString &contact, const QString &hint);

private:
    Q_DECLARE_PRIVATE(LookupService)
    LookupServicePrivate *d_ptr;
};

}

#endif // KPEOPLE_SERVICE_H
