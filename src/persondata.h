/*
    KPeople
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>
    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>

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

#include "kpeople_export.h"

#include <QObject>
#include <QUrl>
#include <QStringList>
#include <KDateTime>

namespace Nepomuk2 { class Resource; }

namespace KPeople
{
struct PersonDataPrivate;

class KPEOPLE_EXPORT PersonData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl avatar READ avatar NOTIFY dataChanged)
    Q_PROPERTY(QString name READ name NOTIFY dataChanged)
    Q_PROPERTY(QString status READ status NOTIFY dataChanged)
    Q_PROPERTY(QStringList emails READ emails NOTIFY dataChanged)
    Q_PROPERTY(QStringList imAccounts READ imAccounts NOTIFY dataChanged)
    Q_PROPERTY(QStringList phones READ phones NOTIFY dataChanged)
    Q_PROPERTY(bool isPerson READ isPerson)
    Q_PROPERTY(bool isValid READ isValid)

    public:
        static PersonData* loadFromUri(const QUrl &url, QObject *parent=0);
        static PersonData* loadFromContactId(const QString &contactId, QObject *parent=0);

        virtual ~PersonData();

        /** Returns if the URI represents a valid person or contact*/
        bool isValid() const;

        /** @returns the uri of the current person */
        QUrl uri() const;

        /** @returns a url pointing to the avatar image */
        QUrl avatar() const;

        /** @returns any possible display name (either from IM, email or other label) */
        QString name() const;

        /** @returns most online status (if there are more than 1 contacts, otherwise the current status of a contact */
        QString status() const;

        /** @returns list of all emails this contact has */
        QStringList emails() const;

        /** @returns list of all phone contacts this contact has */
        QStringList phones() const;

        /** @returns list of all IM accounts this contact has */
        QStringList imAccounts() const;

        /** @returns true if this is pimo:Person, false if just nco:PersonContact */
        bool isPerson() const;
        /** @returns contact's birthday */
        KDateTime birthday() const;

        /** @returns list of groups the contact is part of */
        QStringList groups() const;

        QList<Nepomuk2::Resource> contactResources() const;

    Q_SIGNALS:
        /** Some of the person's data we're offering has changed */
        void dataChanged();

    protected:
        PersonData(QObject *parent);

        /** sets new contact uri, all data are refetched */
        void loadUri(const QUrl &uri);

        /** @p id will specify the person we're offering by finding the pimo:Person related to it */
        void loadContact(const QString &id);

    private:
        Q_DECLARE_PRIVATE(PersonData)
        PersonDataPrivate * d_ptr;

        QString findMostOnlinePresence(const QStringList &presences) const;
};
}

#endif // PERSONDATA_H
