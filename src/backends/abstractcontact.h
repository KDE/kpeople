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

#ifndef KPEOPLE_CONTACT
#define KPEOPLE_CONTACT

#include <QSharedData>
#include <QVariant>
#include <kpeople/kpeople_export.h>

namespace KPeople
{
/**
 * @brief KPeople::AbstractContact is the class to provide the data from a given
 * contact by the backends.
 *
 * To obtain it from a front-end application PersonData and PersonsModel
 * should be used.
 *
 * @internal
 */

class KPEOPLE_EXPORT AbstractContact : public QSharedData
{
public:
    typedef QExplicitlySharedDataPointer<AbstractContact> Ptr;
    typedef QList<AbstractContact::Ptr> List;
    AbstractContact();
    virtual ~AbstractContact();

//     well-known properties
    /** String property representing the display name of the contact */
    static const QString NameProperty;

    /** String property representing the preferred name of the contact */
    static const QString EmailProperty;

    /** String property representing the preferred phone number of the contact */
    static const QString PhoneNumberProperty;

    /** QVariantList property that lists all phone numbers the contact has */
    static const QString AllPhoneNumbersProperty;

    /**
     * String property representing the IM presence of the contact.
     * @sa KPeople::iconNameForPresenceString()
     */
    static const QString PresenceProperty;

    /**
     * QUrl or QPixmap property representing the contacts' avatar
     */
    static const QString PictureProperty;

    /** QVariantList property that lists the groups the contacts belongs to */
    static const QString GroupsProperty;

    /** QVariantList property that lists the emails the contact has */
    static const QString AllEmailsProperty;

    /**
     * Generic method to access a random contact property
     *
     * @returns the value for the @p key property.
     */
    virtual QVariant customProperty(const QString &key) const = 0;

private:
    Q_DISABLE_COPY(AbstractContact)
};

}

Q_DECLARE_METATYPE(KPeople::AbstractContact::List)
Q_DECLARE_METATYPE(KPeople::AbstractContact::Ptr)

#endif
