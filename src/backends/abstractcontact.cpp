/*
    Copyright (C) 2014 Aleix Pol i Gonzalez <aleixpol@blue-systems.com>

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

#include "abstractcontact.h"

using namespace KPeople;

const QString AbstractContact::NameProperty = QStringLiteral("name");
const QString AbstractContact::EmailProperty = QStringLiteral("email");
const QString AbstractContact::PresenceProperty = QStringLiteral("presence");
const QString AbstractContact::AllEmailsProperty = QStringLiteral("all-email");
const QString AbstractContact::PictureProperty = QStringLiteral("picture");
const QString AbstractContact::GroupsProperty = QStringLiteral("all-groups");
const QString AbstractContact::PhoneNumberProperty = QStringLiteral("phoneNumber");
const QString AbstractContact::AllPhoneNumbersProperty = QStringLiteral("all-phoneNumbers");

AbstractContact::AbstractContact()
{
}

AbstractContact::~AbstractContact()
{
}
