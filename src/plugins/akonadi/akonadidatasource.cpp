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

#include "akonadidatasource.h"

#include <Akonadi/Item>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/Collection>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>

#include <KABC/Addressee>

#include <QDebug>

using namespace Akonadi;

AkonadiDataSource::AkonadiDataSource(QObject *parent):
    BasePersonsDataSource(parent)
{
}

AkonadiDataSource::~AkonadiDataSource()
{

}

const KABC::AddresseeList AkonadiDataSource::allContacts()
{
    KABC::AddresseeList addressees;

    CollectionFetchJob *fetchJob = new CollectionFetchJob(Collection::root(), CollectionFetchJob::Recursive, this);
    fetchJob->fetchScope().setContentMimeTypes( QStringList() << "text/directory" );
    fetchJob->exec();
    QList<Collection> contactCollections;
    foreach (const Collection &collection, fetchJob->collections()) {
        if (collection.contentMimeTypes().contains( KABC::Addressee::mimeType() ) ) {
            ItemFetchJob *itemFetchJob = new ItemFetchJob(collection);
            itemFetchJob->fetchScope().fetchFullPayload();
            itemFetchJob->exec();
            foreach (const Item &item, itemFetchJob->items()) {
                if (item.hasPayload<KABC::Addressee>()) {
                    addressees << item.payload<KABC::Addressee>();
                }
            }
        }
    }

    return addressees;
}


const KABC::Addressee AkonadiDataSource::contact(const QString& contactId)
{
    Akonadi::Item item = Akonadi::Item::fromUrl(KUrl(contactId));
    Akonadi::ItemFetchJob *fetchJob = new Akonadi::ItemFetchJob(item, this);
    fetchJob->fetchScope().fetchFullPayload();
    fetchJob->exec();
    if (fetchJob->items().isEmpty()) {
        return KABC::Addressee();
    }

    return fetchJob->items().first().payload<KABC::Addressee>();
}

#include "akonadidatasource.moc"
