/*
    Persons Model
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>
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


#include "persons-model.h"
#include "persons-model-item.h"
#include "persons-model-contact-item.h"
#include "person-cache.h"
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Variant>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/StoreResourcesJob>
#include <KDebug>

// class PersonsModelPrivate {
// public:
// };

PersonsModel::PersonsModel(QObject *parent, bool init)
    : QStandardItemModel(parent)
//     , d_ptr(new PersonsModelPrivate)
{
    QHash<int, QByteArray> names = roleNames();
    names.insert(PersonsModel::EmailRole, "email");
    names.insert(PersonsModel::PhoneRole, "phone");
    names.insert(PersonsModel::ContactIdRole, "contactId");
    names.insert(PersonsModel::ContactTypeRole, "contactType");
    names.insert(PersonsModel::IMRole, "im");
    names.insert(PersonsModel::NickRole, "nick");
    names.insert(PersonsModel::UriRole, "uri");
    names.insert(PersonsModel::NameRole, "name");
    names.insert(PersonsModel::PhotoRole, "photo");
    setRoleNames(names);
    
    if(init) {
        PersonCache* cache = new PersonCache(this);
    
        connect(cache,
                SIGNAL(contactsFetched(QList<PersonsModelItem*>,QList<PersonsModelContactItem*>)),
                SLOT(init(QList<PersonsModelItem*>,QList<PersonsModelContactItem*>)));
        cache->startQuery();
    }
}

template <class T>
QList<QStandardItem*> toStandardItems(const QList<T*>& items)
{
    QList<QStandardItem*> ret;
    foreach(QStandardItem* it, items) {
        ret += it;
    }
    return ret;
}

void PersonsModel::init(const QList<PersonsModelItem*>& people, const QList<PersonsModelContactItem*>& other)
{
    QStandardItem* root = invisibleRootItem();
    root->appendRows(toStandardItems(people));
    root->appendRows(toStandardItems(other));
    emit peopleAdded();
}

void PersonsModel::unmerge(const QUrl& contactUri)
{
    Nepomuk2::SimpleResource personContact;
    personContact.addType( Nepomuk::Vocabulary::PIMO::Person() );
    
    Nepomuk2::SimpleResource res(contactUri);
    res.setProperty(Nepomuk::Vocabulary::PIMO::groundingOccurrence(), personContact.uri());
    
    Nepomuk2::SimpleResourceGraph graph;
    graph << res << personContact;

    Nepomuk2::StoreResourcesJob * job = Nepomuk2::storeResources( graph );
    job->setProperty("uri", contactUri);
    connect(job, SIGNAL(finished(KJob*)), SLOT(unmergeFinished(KJob*)));
    job->start();
}

void PersonsModel::unmergeFinished(KJob* job)
{
    if(job->error()!=0) {
        kWarning() << "Unmerge failed for "<<job->property("uri").toString();
    }
}
