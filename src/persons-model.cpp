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
#include "resource-watcher-service.h"
#include <Soprano/Query/QueryLanguage>
#include <Soprano/QueryResultIterator>
#include <Soprano/Model>

#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/Variant>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/StoreResourcesJob>
#include <KDebug>

struct PersonsModelPrivate {
    QHash<QUrl, PersonsModelItem*> persons;
};

PersonsModel::PersonsModel(QObject *parent, bool init)
    : QStandardItemModel(parent)
    , d_ptr(new PersonsModelPrivate)
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
    names.insert(PersonsModel::ContactsCount, "contactsCount");
    setRoleNames(names);
    
    if(init) {
        QMetaObject::invokeMethod(this, "query", Qt::QueuedConnection);
        new ResourceWatcherService(this);
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

QHash<QString, QUrl> initUriToBinding()
{
    QHash<QString, QUrl> ret;
    QList<QUrl> list;
    list
    << Nepomuk2::Vocabulary::NCO::imNickname()
    << Nepomuk2::Vocabulary::NCO::imAccountType()
    << Nepomuk2::Vocabulary::NCO::imID()
    //                      << Nepomuk2::Vocabulary::Telepathy::statusType()
    //                      << Nepomuk2::Vocabulary::Telepathy::accountIdentifier()
    << QUrl(QLatin1String("http://nepomuk.kde.org/ontologies/2009/06/20/telepathy#statusType"))
    << Nepomuk2::Vocabulary::NCO::imStatus()
    << Nepomuk2::Vocabulary::NCO::hasEmailAddress()
    << Nepomuk2::Vocabulary::NCO::emailAddress();
    
    foreach(const QUrl& keyUri, list) {
        QString keyString = keyUri.toString();
        //convert every key to correspond to the nepomuk bindings
        keyString = keyString.mid(keyString.lastIndexOf(QLatin1Char('/')) + 1).replace(QLatin1Char('#'), QLatin1Char('_'));
        ret[keyString] = keyUri;
    }
    return ret;
}

void PersonsModel::query()
{
    Q_ASSERT(rowCount()==0);
    Q_D(PersonsModel);
    QHash<QString, QUrl> uriToBinding = initUriToBinding();

    QString nco_query = QString::fromUtf8("select ?uri ?pimo_groundingOccurance ?nco_hasIMAccount"
                      "?nco_imNickname ?telepathy_statusType ?nco_imID ?nco_imAccountType ?nco_hasEmailAddress"
                      "?nco_imStatus "

                      "WHERE { "
                            "?uri a nco:PersonContact ."
                            "?pimo_groundingOccurance  pimo:groundingOccurrence    ?uri . "

//                       "OPTIONAL { "
                            "?uri                       nco:hasIMAccount            ?nco_hasIMAccount ."
                            "?nco_hasIMAccount          nco:imNickname              ?nco_imNickname ."
                            "?nco_hasIMAccount          telepathy:statusType        ?telepathy_statusType ."
                            "?nco_hasIMAccount          nco:imStatus                ?nco_imStatus ."
                            "?nco_hasIMAccount          nco:imID                    ?nco_imID ."
                            "?nco_hasIMAccount          nco:imAccountType           ?nco_imAccountType ."
//                       " } "
                      
                      "OPTIONAL { "
                            "?uri                       nco:hasEmailAddress         ?nco_hasEmailAddress . "
                            "?nco_hasEmailAddress       nco:emailAddress            ?nco_emailAddress . "
                      " } "
                "}");

    Soprano::Model* m = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = m->executeQuery(nco_query,
                                                      Soprano::Query::QueryLanguageSparql);

    while(it.next()) {
        QUrl currentUri = it[QLatin1String("uri")].uri();
        QString display /*= it[QLatin1String("nao_prefLabel")].toString()*/;
        PersonsModelContactItem* contactNode = new PersonsModelContactItem(currentUri, display);

        
        for(QHash<QString, QUrl>::const_iterator iter=uriToBinding.constBegin(), itEnd=uriToBinding.constEnd(); iter!=itEnd; ++iter) {
            contactNode->addData(iter.value(), it[iter.key()].toString());
        }

        QUrl pimoPersonUri = it[QLatin1String("pimo_groundingOccurance")].uri();
        Q_ASSERT(!pimoPersonUri.isEmpty());
        QHash< QUrl, PersonsModelItem* >::const_iterator pos = d->persons.constFind(pimoPersonUri);
        if (pos == d->persons.constEnd())
            pos = d->persons.insert(pimoPersonUri, new PersonsModelItem(pimoPersonUri));
        pos.value()->appendRow(contactNode);
    }

    invisibleRootItem()->appendRows(toStandardItems(d->persons.values()));
    emit peopleAdded();
}

void PersonsModel::unmerge(const QUrl& contactUri, const QUrl& personUri)
{
    Nepomuk2::Resource oldPerson(personUri);
    Q_ASSERT(oldPerson.property(Nepomuk2::Vocabulary::PIMO::groundingOccurrence()).toUrlList().size()>=2 && "there's nothing to unmerge...");
    oldPerson.removeProperty(Nepomuk2::Vocabulary::PIMO::groundingOccurrence(), contactUri);
    
    Nepomuk2::SimpleResource newPerson;
    newPerson.addType( Nepomuk2::Vocabulary::PIMO::Person() );
    newPerson.setProperty(Nepomuk2::Vocabulary::PIMO::groundingOccurrence(), contactUri);
    
    Nepomuk2::SimpleResourceGraph graph;
    graph << newPerson;

    KJob * job = Nepomuk2::storeResources( graph );
    job->setProperty("uri", contactUri);
    job->setObjectName("Unmerge");
    connect(job, SIGNAL(finished(KJob*)), SLOT(jobFinished(KJob*)));
    job->start();
}

void PersonsModel::merge(const QList< QUrl >& persons)
{
    KJob* job = Nepomuk2::mergeResources( persons );
    job->setObjectName("Merge");
    connect(job, SIGNAL(finished(KJob*)), SLOT(jobFinished(KJob*)));
}

void PersonsModel::merge(const QVariantList& persons)
{
    QList<QUrl> conv;
    foreach(const QVariant& p, persons)
        conv += p.toUrl();
    merge(conv);
}

void PersonsModel::jobFinished(KJob* job)
{
    if(job->error()!=0) {
        kWarning() << job->objectName() << " failed for "<< job->property("uri").toString() << job->errorText() << job->errorString();
    } else {
        kWarning() << job->objectName() << " done: "<< job->property("uri").toString();
    }
}

QModelIndex PersonsModel::indexForUri(const QUrl& uri) const
{
    return QModelIndex();
}
