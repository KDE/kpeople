/*
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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QStringList>
#include <QIcon>

#include "kpeople_export.h"

#include <KABC/Addressee>

class QAction;
namespace KPeople
{
    /**
     * Merge all ids into a single person.
     * Ids can be a mix of person Ids and contact IDs.
     *
     * @arg ids a list of all identifiers to be merged
     *
     * @return the identifier of the new person or an empty string upon failure
     */
    KPEOPLE_EXPORT QString mergeContacts(const QStringList &ids);

    /**
     * Unmerge a contact. Either remove a contact from a given person or remove a person
     *
     * @return Whether the unmerge was successful
     */
    KPEOPLE_EXPORT bool unmergeContact(const QString &id);

    KPEOPLE_EXPORT QList<QAction*> actionsForPerson(const KABC::Addressee &person,
                                                    const KABC::AddresseeList &contacts,
                                                    QObject *parent);

    /**
     * Return a QPixmap for a TP presence string
     *
     * @return QPixmap with the Tp presence icon
     */
    KPEOPLE_EXPORT QString iconNameForPresenceString(const QString &presenceName);

    /**
     * Returns a sort priority for the given presenceName
     *
     * @return sort priority of the given presence
     */
    KPEOPLE_EXPORT int presenceSortPriority(const QString &presenceName);

};

#endif // GLOBAL_H
