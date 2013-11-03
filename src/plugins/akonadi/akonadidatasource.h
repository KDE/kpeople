/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  David Edmundson <davidedmundson@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef AKONADIDATASOURCE_H
#define AKONADIDATASOURCE_H

#include <basepersonsdatasource.h>

#include <Akonadi/Monitor>

class AkonadiDataSource : public KPeople::BasePersonsDataSource
{
    Q_OBJECT

public:
    AkonadiDataSource(QObject *parent=0);
    virtual ~AkonadiDataSource();

    virtual const KABC::Addressee::Map allContacts();
    virtual const KABC::Addressee contact(const QString& contactId);
private Q_SLOTS:
    void onItemAdded(const Akonadi::Item &item);
    void onItemChanged(const Akonadi::Item &item);
    void onItemRemoved(const Akonadi::Item &item);
private:
    Akonadi::Monitor *m_monitor;
};

#endif // AKONADIDATASOURCE_H
