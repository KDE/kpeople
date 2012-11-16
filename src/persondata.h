/*
    KPeople
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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

class QModelIndex;
namespace Nepomuk2 { class Resource; }
struct PersonDataPrivate;

class KPEOPLE_EXPORT PersonData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString contactId READ contactId WRITE setContactId NOTIFY contactChanged)
    Q_PROPERTY(QUrl uri READ uri NOTIFY contactChanged)

    Q_PROPERTY(QUrl avatar READ avatar NOTIFY dataChanged)
    Q_PROPERTY(QString imNickname READ nickname NOTIFY dataChanged)
    Q_PROPERTY(QString status READ status NOTIFY dataChanged)
    Q_PROPERTY(QStringList contacts READ contacts NOTIFY dataChanged)
    public:
        explicit PersonData(QObject *parent = 0);

        /** @returns the uri of the current person */
        QUrl uri() const;

        /** @p id will specify the person we're offering by finding the pimo:Person related to it */
        void setContactId(const QString &id);
        QString contactId() const;

        /** @returns a url pointing to the avatar image */
        QUrl avatar() const;

        QString nickname() const;

        QString status() const;

        /** @returns a list of contact id's for the current person */
        QStringList contacts() const;

        /** @returns a model index so that we can query it like it was from PersonsModel */
        QModelIndex personIndex() const;

        /** @p uri uri of the nepomuk resource (either contact or person) */
        void setContactUri(const QUrl &uri);

    private slots:
        void personInitialized();

    signals:
        //FIXME: Shouldn't have such a demanding API...
        /** The data has been initialized, now we can start pulling data. */
        void dataInitialized();

        /** The person we're pointing to has changed */
        void contactChanged();

        /** Some of the person's data we're offering has changed */
        void dataChanged();

    private:
        Q_DECLARE_PRIVATE(PersonData)
        PersonDataPrivate * d_ptr;
};

#endif // PERSONDATA_H
