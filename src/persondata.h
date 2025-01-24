/*
    KPeople
    SPDX-FileCopyrightText: 2013 David Edmundson (davidedmundson@kde.org)

    SPDX-License-Identifier: LGPL-2.1-or-later
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

/*!
 * \qmlvaluetype personData
 * \inqmlmodule org.kde.kpeople
 * \nativetype KPeople::PersonData
 *
 * \brief Allows querying the information about a given person.
 *
 * PersonData exposes the information of a given person (in contrast to everyone
 * available, which is done by PersonsModel).
 * This class will provide comfortable interfaces so it can be easily adopted
 * in any application.
 */

/*!
 * \class KPeople::PersonData
 * \inmodule KPeople
 * \inheaderfile KPeople/PersonData
 *
 * \brief Allows querying the information about a given person.
 *
 * PersonData exposes the information of a given person (in contrast to everyone
 * available, which is done by PersonsModel).
 * This class will provide comfortable interfaces so it can be easily adopted
 * in any application.
 *
 * \since 5.8
 */
class KPEOPLE_EXPORT PersonData : public QObject
{
    Q_OBJECT

    /*!
     * \qmlproperty string personData::name
     */

    /*!
     * \property KPeople::PersonData::name
     */
    Q_PROPERTY(QString name READ name NOTIFY dataChanged)

    /*!
     * \qmlproperty image personData::photo
     */

    /*!
     * \property KPeople::PersonData::photo
     */
    Q_PROPERTY(QPixmap photo READ photo NOTIFY dataChanged)

    /*!
     * \qmlproperty string personData::presenceIconName
     */

    /*!
     * \property KPeople::PersonData::presenceIconName
     */
    Q_PROPERTY(QString presenceIconName READ presenceIconName NOTIFY dataChanged)

    /*!
     * \qmlproperty bool personData::isEditable
     */

    /*!
     * \property KPeople::PersonData::isEditable
     *
     * Returns whether setContactCustomProperty can be called on this contact
     *
     * \since 5.62
     */
    Q_PROPERTY(bool isEditable READ isEditable CONSTANT)

public:
    /*! Creates a Person object from a given ID.
     * The ID can be either a local application specific ID (such as akonadi://?item=15)
     * or a kpeople ID in the form kpeople://15
     */
    PersonData(const QString &id, QObject *parent = nullptr);

    ~PersonData() override;

    /*!
     * Returns true if this PersonData is mapped to some existing contact
     * \since 5.22
     */
    bool isValid() const;

    /*! Returns the person's id */
    QString personUri() const;

    /*!
     * Returns a list of contact ids that identify the PersonData instance.
     */
    QStringList contactUris() const;

    /*!
     * Returns the name of the person
     */
    QString name() const;

    /*!
     * Returns an icon name that represents the IM status of the person
     */
    QString presenceIconName() const;

    /*!
     * Returns a pixmap with the photo of the person, or a default one if not available
     */
    QPixmap photo() const;

    /*!
     * Returns the property for a said \a key.
     */
    Q_SCRIPTABLE QVariant contactCustomProperty(const QString &key) const;

    /*!
     * Sends a desired \a value for the contact according to the \a key.
     * It's not necessarily implemented. The back-end gets to decide whether a property
     * can be set or not.
     *
     * Returns whether the property value was changed
     *
     * \since 5.62
     */
    Q_SCRIPTABLE bool setContactCustomProperty(const QString &key, const QVariant &value);

    /*!
     * Returns the contact's online presence.
     */
    QString presence() const;

    /*!
     * Returns the contact's preferred email address.
     */
    QString email() const;

    /*!
     * Returns a the url of the picture that represents the contact.
     */
    QUrl pictureUrl() const;

    /*! Returns all groups the person is in. */
    QStringList groups() const;

    /*! Returns all e-mail addresses from the person. */
    QStringList allEmails() const;

    /*!
     * Returns whether the contact can be edited.
     *
     * \since 5.62
     */
    bool isEditable() const;

    //     struct PhoneNumber {
    //         QString name;
    //         QString number;
    //     };
    //     QList<PhoneNumber> phoneNumbers() const { createPhoneNumbers(customProperty("phoneNumbers")); };

Q_SIGNALS:
    /*!
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
