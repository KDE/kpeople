/*
    Persons Model
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>
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

#ifndef PERSONS_MODEL_H
#define PERSONS_MODEL_H

#include <kpeople/kpeople_export.h>
#include <QAbstractItemModel>

#include "global.h"

namespace KPeople
{
class ContactItem;
class PersonItem;
class MetaContact;
class PersonsModelPrivate;

/**
 * This class creates a model of all known contacts from all sources
 * Contacts are represented as a tree where the top level represents a "person" which is an
 * amalgamation of all the sub-contacts
 *
 * @since 5.8
 */
class KPEOPLE_EXPORT PersonsModel : public QAbstractItemModel
{
    Q_OBJECT
    /** specifies whether the model has already been initialized */
    Q_PROPERTY(bool isInitialized READ isInitialized NOTIFY modelInitialized)
public:
    enum Role {
        FormattedNameRole = Qt::DisplayRole,//QString best name for this person
        PhotoRole = Qt::DecorationRole, //QPixmap best photo for this person
        PersonUriRole = Qt::UserRole, //QString ID of this person
        PersonVCardRole, //AbstractContact::Ptr
        ContactsVCardRole, //AbstractContact::List (FIXME or map?)

        GroupsRole, ///groups QStringList

        UserRole = Qt::UserRole + 0x1000 ///< in case it's needed to extend, use this one to start from
    };
    Q_ENUM(Role)

    PersonsModel(QObject *parent = nullptr);

    virtual ~PersonsModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    /** @returns the index for a given @p personUri */
    QModelIndex indexForPersonUri(const QString &personUri) const;

    /** Returns if all the backends have been initialized yet. */
    bool isInitialized() const;

    /** Helper class to ease model access through QML */
    Q_SCRIPTABLE QVariant get(int row, int role);

    /**
     * Makes it possible to access custom properties that are not available to the model
     *
     * @returns the property for the contact at @p index defined by the @p key
     */
    QVariant contactCustomProperty(const QModelIndex &index, const QString &key) const;

Q_SIGNALS:
    /** Will emit when the model is finally initialized. @p success will specify if it succeeded */
    void modelInitialized(bool success);

private:
    Q_DISABLE_COPY(PersonsModel)

    QScopedPointer<PersonsModelPrivate> const d_ptr;
    Q_DECLARE_PRIVATE(PersonsModel)
};
}

#endif // PERSONS_MODEL_H
