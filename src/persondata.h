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

#include <kpeople/kpeople_export.h>

#include <QObject>
#include <QPixmap>

#include "global.h"

namespace KPeople
{
class PersonDataPrivate;

class PersonData;

/**
 * @brief Allows to query the information about a given person
 *
 * PersonData exposes the information of a given person (in contrast to everyone
 * available, which is done by PersonsModel).
 * This class will provide comfortable interfaces so it can be easily adopted
 * in any application.
 */
class KPEOPLE_EXPORT PersonData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY dataChanged)
    Q_PROPERTY(QPixmap photo READ photo NOTIFY dataChanged)
    Q_PROPERTY(QString presenceIconName READ presenceIconName NOTIFY dataChanged)

public:
    /** Creates a Person object from a given ID.
     * The ID can be either a local application specific ID (such as akonadi://?item=15)
     * or a kpeople ID in the form kpeople://15
     */
    PersonData(const QString &id, QObject *parent = 0);

    virtual ~PersonData();

    /** Returns the person's id */
    QString personId() const;

    /**
     * Returns a list of contact ids that identify the PersonData instance.
     */
    QStringList contactIds() const;

    /**
     * @returns the name of the person
     */
    QString name() const;

    /**
     * @returns an icon name that represents the IM status of the person
     */
    QString presenceIconName() const;

    /**
     * @returns a pixmap with the photo of the person, or a default one if not available
     */
    QPixmap photo() const;

    /**
     * @returns the property for a said @p key.
     */
    QVariant contactCustomProperty(const QString &key) const;

    /**
    * Returns the contact's online presence.
    */
    QString presence() const;

    /**
    * Returns the contact's preferred email address.
    */
    QString email() const;

    /**
    * Returns a the url of the picture that represents the contact.
    */
    QUrl pictureUrl() const;

    /** Returns all groups the person is in. */
    QStringList groups() const;

    /** Returns all e-mail addresses from the person. */
    QStringList allEmails() const;

    //     struct PhoneNumber {
    //         QString name;
    //         QString number;
    //     };
    //     QVector<PhoneNumber> phoneNumbers() const { createPhoneNumbers(customProperty("phoneNumbers")); };

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
    PersonDataPrivate *d_ptr;
};
}

#endif // PERSONDATA_H
