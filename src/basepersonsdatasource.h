/*
 *    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#ifndef BASE_PERSONS_DATA_SOURCE_H
#define BASE_PERSONS_DATA_SOURCE_H

#include <QObject>
#include <QVariant>
#include <KABC/Addressee>

#include "kpeople_export.h"

namespace KPeople
{

    //later
//     FetchContactsJob() : public KJob
// {
//
//     QMap<id, KABC::Addressee> contacts();
// } and the virtual methods return this.

class KPEOPLE_EXPORT BasePersonsDataSource : public QObject
{
    Q_OBJECT
public:
    BasePersonsDataSource(QObject *parent = 0, const QVariantList &args = QVariantList());
    virtual ~BasePersonsDataSource();

    //TODO make this async
    virtual const KABC::Addressee::Map allContacts();

    //TODO make this async + possibly take a list
    virtual const KABC::Addressee contact(const QString &contactId);

Q_SIGNALS:
    void contactChanged(const QString &contactId);
    void contactAdded(const QString &contactId);
    void contactRemoved(const QString &contactId);

};

}
#endif // BASE_PERSONS_DATA_SOURCE_H
