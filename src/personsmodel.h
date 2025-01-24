/*
    Persons Model
    SPDX-FileCopyrightText: 2012 Martin Klapetek <martin.klapetek@gmail.com>
    SPDX-FileCopyrightText: 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
    SPDX-FileCopyrightText: 2013 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef PERSONS_MODEL_H
#define PERSONS_MODEL_H

#include <kpeople/kpeople_export.h>

#include "global.h"

#include <QAbstractItemModel>

namespace KPeople
{
class ContactItem;
class PersonItem;
class MetaContact;
class PersonsModelPrivate;

/*!
 * \qmlvaluetype personsModel
 * \inqmlmodule org.kde.kpeople
 * \nativetype KPeople::PersonsModel
 *
 * \brief This class creates a model of all known contacts from all sources.
 *
 * Contacts are represented as a tree where the top level represents a "person" which is an
 * amalgamation of all the sub-contacts
 */

/*!
 * \class KPeople::PersonsModel
 * \inmodule KPeople
 * \inheaderfile KPeople/PersonsModel
 *
 * \brief This class creates a model of all known contacts from all sources.
 *
 * Contacts are represented as a tree where the top level represents a "person" which is an
 * amalgamation of all the sub-contacts
 *
 * \since 5.8
 */
class KPEOPLE_EXPORT PersonsModel : public QAbstractItemModel
{
    Q_OBJECT

    /*!
     * \qmlproperty bool personsModel::isInitialized
     */

    /*!
     * \property KPeople::PersonsModel::isInitialized
     */
    Q_PROPERTY(bool isInitialized READ isInitialized NOTIFY modelInitialized)

public:
    /*!
     * \enum KPeople::PersonsModel::Role
     *
     * \value FormattedNameRole
     * QString best name for this person
     *
     * \value PhotoRole
     * QPixmap best photo for this person
     *
     * \value PersonUriRole
     * QString ID of this person
     *
     * \value PersonVCardRole
     * AbstractContact::Ptr
     *
     * \value ContactsVCardRole
     * AbstractContact::List (FIXME or map?)
     *
     * \value GroupsRole
     * QStringList
     *
     * \value PhoneNumberRole
     *
     * \value PhotoImageProviderUri
     * Provide a URL to use with QtQuick's Image.source, similar to the Photo Role. \since 5.93

     * \value UserRole
     * In case it's needed to extend, use this one to start from
     */
    enum Role {
        FormattedNameRole = Qt::DisplayRole,
        PhotoRole = Qt::DecorationRole,
        PersonUriRole = Qt::UserRole,
        PersonVCardRole,
        ContactsVCardRole,

        GroupsRole,
        PhoneNumberRole,
        PhotoImageProviderUri,

        UserRole = Qt::UserRole + 0x1000,
    };
    Q_ENUM(Role)

    explicit PersonsModel(QObject *parent = nullptr);

    ~PersonsModel() override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    /*! Returns the index for a given \a personUri */
    QModelIndex indexForPersonUri(const QString &personUri) const;

    /*! Returns if all the backends have been initialized yet. */
    bool isInitialized() const;

    /*! Helper class to ease model access through QML */
    Q_SCRIPTABLE QVariant get(int row, int role);

    /*!
     * Makes it possible to access custom properties that are not available to the model
     *
     * Returns the property for the contact at \a index defined by the \a key
     */
    QVariant contactCustomProperty(const QModelIndex &index, const QString &key) const;

Q_SIGNALS:
    /*! Will emit when the model is finally initialized. \a success will specify if it succeeded */
    void modelInitialized(bool success);

private:
    Q_DISABLE_COPY(PersonsModel)

    QScopedPointer<PersonsModelPrivate> const d_ptr;
    Q_DECLARE_PRIVATE(PersonsModel)
};
}

#endif // PERSONS_MODEL_H
