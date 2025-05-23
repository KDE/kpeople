/*
    SPDX-FileCopyrightText: 2013 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QStringList>
#include <QVariantMap>

#include <kpeople/kpeople_export.h>

/*!
 * \namespace KPeople
 * \inmodule KPeople
 */
namespace KPeople
{
class PersonData;

/*!
 * Merge all uris into a single person.
 *
 * Ids can be a mix of person Ids and contact IDs.
 *
 * \a uris a list of all identifiers to be merged
 *
 * Returns the identifier of the new person or an empty string upon failure
 */
KPEOPLE_EXPORT QString mergeContacts(const QStringList &uris);

/*!
 * Unmerge a contact. Either remove a contact from a given person or remove a person
 * identified by @p uri.
 *
 * Returns whether the unmerge was successful
 */
KPEOPLE_EXPORT bool unmergeContact(const QString &uri);

/*!
 * Return a QPixmap for a TP presence string
 */
KPEOPLE_EXPORT QString iconNameForPresenceString(const QString &presenceName);

/*!
 * Returns a sort priority for the given presenceName
 */
KPEOPLE_EXPORT int presenceSortPriority(const QString &presenceName);

}

void initResources();

#endif // GLOBAL_H
