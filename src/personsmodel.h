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
#include <kpeoplebackend/abstractcontact.h>
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
        PersonIdRole = Qt::UserRole, //QString ID of this person
        PersonVCardRole, //AbstractContact::Ptr
        ContactsVCardRole, //AbstractContact::List (FIXME or map?)

        GroupsRole, ///groups QStringList

        UserRole = Qt::UserRole + 0x1000 ///< in case it's needed to extend, use this one to start from
    };
    Q_ENUMS(Role)

    PersonsModel(QObject *parent = 0);

    virtual ~PersonsModel();

    virtual int columnCount (const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    /** @returns the index for a given @p personId */
    QModelIndex indexForPersonId(const QString& personId) const;

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
    void modelInitialized(bool success);

private Q_SLOTS:
    void onContactsFetched();

    //update when a resource signals a contact has changed
    void onContactAdded(const QString &contactId, const AbstractContact::Ptr &contact);
    void onContactChanged(const QString &contactId, const AbstractContact::Ptr &contact);
    void onContactRemoved(const QString &contactId);

    //update on metadata changes
    void onAddContactToPerson(const QString &contactId, const QString &newPersonId);
    void onRemoveContactsFromPerson(const QString &contactId);

    void onMonitorInitialFetchComplete(bool success = true);

private:
    Q_DISABLE_COPY(PersonsModel)

    //methods that manipulate the model
    void addPerson(const MetaContact &mc);
    void removePerson(const QString &id);
    void personChanged(const QString &personId);

    QString personIdForContact(const QString &contactId) const;
    QVariant dataForAddressee(const QString &personId, const AbstractContact::Ptr &contact, int role) const;

    PersonsModelPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(PersonsModel);
};
}

// Q_DECLARE_METATYPE(AbstractContact::Ptr)
Q_DECLARE_METATYPE(KPeople::AbstractContact::List)

#endif // PERSONS_MODEL_H
