/*
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>
    Copyright 2013  Martin Klapetek <mklapetek@kde.org>

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

#ifndef PERSONMANAGER_H
#define PERSONMANAGER_H

#include <QObject>
#include <QStringList>
#include <QMultiHash>

#include <QSqlDatabase>
#include <QSqlQuery>

#include <kpeople/kpeople_export.h>

/**
 * This is a private internal class that manages all the internal mapping of contacts <---> persons
 * It stores the connection to the database as well as signals communicating with other clients
 *
 * It is a singleton.
 *
 */

class KPEOPLE_EXPORT PersonManager : public QObject
{
    Q_OBJECT

public:
    /**
     * Create or return a singleton instance of the PersonManager
     *
     * @databasePath the path to the database to be used.
     * If null the correct database is used.
     * This is for testing purposes only.
     */
    static PersonManager *instance(const QString &databasePath = QString());

//DATA RETRIEVAL------------

    /** Retuns a list of all known personIDs in the database*/
    QMultiHash< QString /*PersonUri*/, QString /*ContactUri*/> allPersons() const;

    /**
     * Returns the ID of a person associated with a given contact
     * If no person for that contact exists, an empty string is returned
     */
    QString personUriForContact(const QString &contactUri) const;

    /**
     * Returns a list of contactUris associated with a given person
     */
    QStringList contactsForPersonUri(const QString &personUri) const;

public Q_SLOTS:
    //merge all ids (person IDs and contactUris into a single person)
    //returns the ID that will be created
    //users should KPeople::mergeContacts from global.h
    QString mergeContacts(const QStringList &ids);

    //unmerge a contact. Either remove a contact from a given person or remove a person
    //users should KPeople::unmergeContact from global.h
    bool unmergeContact(const QString &id);

Q_SIGNALS:
    void contactRemovedFromPerson(const QString &contactUri);
    void contactAddedToPerson(const QString &contactUri, const QString &newPersonUri);

protected:
    explicit PersonManager(const QString &databasePath, QObject *parent = nullptr);
    virtual ~PersonManager();

private:
    QSqlDatabase m_db;
};

#endif // PERSONMANAGER_H
