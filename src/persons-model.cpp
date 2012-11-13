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
#include "nepomuk-tp-channel-delegate.h"

#include <Soprano/Query/QueryLanguage>
#include <Soprano/QueryResultIterator>
#include <Soprano/Model>

#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/Variant>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/StoreResourcesJob>
#include <KDebug>

struct PersonsModelPrivate {
    PersonsModelPrivate() : ktpDelegate(0) {}

    void initKTPDelegate() { if(!ktpDelegate) ktpDelegate = new NepomukTpChannelDelegate; }

    NepomukTpChannelDelegate *ktpDelegate;
};

PersonsModel::PersonsModel(QObject *parent, bool init, const QString& customQuery)
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
    names.insert(PersonsModel::ResourceTypeRole, "resourceType");
    setRoleNames(names);

    if(init) {
        QString nco_query = customQuery;
        if(customQuery.isEmpty())
            nco_query = QString::fromUtf8(
            "select DISTINCT ?uri ?pimo_groundingOccurrence ?nco_hasIMAccount"
                "?nco_imNickname ?telepathy_statusType ?nco_imID ?nco_imAccountType ?nco_hasEmailAddress"
                "?nco_imStatus ?nie_url "

                "WHERE { "
                    "?uri a nco:PersonContact. "

                "OPTIONAL { ?pimo_groundingOccurrence  pimo:groundingOccurrence     ?uri. }"
                "OPTIONAL { "
                    "?uri                       nco:hasIMAccount            ?nco_hasIMAccount. "
                    "OPTIONAL { ?nco_hasIMAccount          nco:imNickname              ?nco_imNickname. } "
                    "OPTIONAL { ?nco_hasIMAccount          telepathy:statusType        ?telepathy_statusType. } "
                    "OPTIONAL { ?nco_hasIMAccount          nco:imStatus                ?nco_imStatus. } "
                    "OPTIONAL { ?nco_hasIMAccount          nco:imID                    ?nco_imID. } "
                    "OPTIONAL { ?nco_hasIMAccount          nco:imAccountType           ?nco_imAccountType. } "
                " } "
                "OPTIONAL {"
                    "?uri                       nco:photo                   ?phRes. "
                    "?phRes                     nie:url                     ?nie_url. "
                " } "
                "OPTIONAL { "
                    "?uri                       nco:hasEmailAddress         ?nco_hasEmailAddress. "
                    "?nco_hasEmailAddress       nco:emailAddress            ?nco_emailAddress. "
                " } "
            "}");

        QMetaObject::invokeMethod(this, "query", Qt::QueuedConnection, Q_ARG(QString, nco_query));
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
    << Nepomuk2::Vocabulary::NCO::hasIMAccount()
    << Nepomuk2::Vocabulary::NCO::emailAddress()
    << Nepomuk2::Vocabulary::NIE::url();

    foreach(const QUrl& keyUri, list) {
        QString keyString = keyUri.toString();
        //convert every key to correspond to the nepomuk bindings
        keyString = keyString.mid(keyString.lastIndexOf(QLatin1Char('/')) + 1).replace(QLatin1Char('#'), QLatin1Char('_'));
        ret[keyString] = keyUri;
    }
    return ret;
}

void PersonsModel::query(const QString& nco_query)
{
    Q_ASSERT(rowCount()==0);
    QHash<QString, QUrl> uriToBinding = initUriToBinding();

    Soprano::Model* m = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = m->executeQuery(nco_query, Soprano::Query::QueryLanguageSparql);
    QHash<QUrl, PersonsModelContactItem*> contacts;
    QHash<QUrl, PersonsModelItem*> persons;

    while(it.next()) {
        QUrl currentUri = it[QLatin1String("uri")].uri();
        if (currentUri == QUrl(QLatin1String("nepomuk:/me"))) {
            continue;
        }
        PersonsModelContactItem* contactNode = contacts.value(currentUri);
        bool newContact = !contactNode;
        if(!contactNode) {
            contactNode = new PersonsModelContactItem(currentUri);
        }

        for(QHash<QString, QUrl>::const_iterator iter=uriToBinding.constBegin(), itEnd=uriToBinding.constEnd(); iter!=itEnd; ++iter) {
            contactNode->addData(iter.value(), it[iter.key()].toString());
        }

        QUrl pimoPersonUri = it[QLatin1String("pimo_groundingOccurrence")].uri();
        if (pimoPersonUri == QUrl(QLatin1String("nepomuk:/me"))) {
            continue;
        }

        if (pimoPersonUri.isEmpty()) {
            //TODO: look for other contacts and possibly automerge
            contacts.insert(currentUri, contactNode);
        } else {
            QHash< QUrl, PersonsModelItem* >::const_iterator pos = persons.constFind(pimoPersonUri);
            if (pos == persons.constEnd()) {
                pos = persons.insert(pimoPersonUri, new PersonsModelItem(pimoPersonUri));
            }
            pos.value()->appendRow(contactNode);
        }
    }

    invisibleRootItem()->appendRows(toStandardItems(persons.values()));
    invisibleRootItem()->appendRows(toStandardItems(contacts.values()));
    emit peopleAdded();
    d_ptr->initKTPDelegate();
    kDebug() << "Model ready";
}

void PersonsModel::unmerge(const QUrl& contactUri, const QUrl& personUri)
{
    Nepomuk2::Resource oldPerson(personUri);
    Q_ASSERT(oldPerson.property(Nepomuk2::Vocabulary::PIMO::groundingOccurrence()).toUrlList().size()>=2 && "there's nothing to unmerge...");
    oldPerson.removeProperty(Nepomuk2::Vocabulary::PIMO::groundingOccurrence(), contactUri);
    if (!oldPerson.hasProperty(Nepomuk2::Vocabulary::PIMO::groundingOccurrence())) {
        oldPerson.remove();
    }

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

QModelIndex PersonsModel::findRecursively(int role, const QVariant& value, const QModelIndex& idx) const
{
    if(idx.isValid() && data(idx, role)==value)
        return idx;
    int rows = rowCount(idx);
    for(int i=0; i<rows; i++) {
        QModelIndex ret = findRecursively(role, value, index(i, 0, idx));
        if(ret.isValid())
            return ret;
    }
    return QModelIndex();
}

QModelIndex PersonsModel::indexForUri(const QUrl& uri) const
{
    return findRecursively(PersonsModel::UriRole, uri);
}

void PersonsModel::createPerson(const Nepomuk2::Resource& res)
{
    Q_ASSERT(!indexForUri(res.uri()).isValid());
    //pass only the uri as that will not add the contacts from groundingOccurrence
    //rationale: if we're adding contacts to the person, we need to check the model
    //           if those contacts are not already in the model and if they are,
    //           we need to remove them from the toplevel and put them only under
    //           the person item. In the time of creation, the model() call from
    //           PersonsModelItem is null, so we cannot check the model.
    //           Furthermore this slot is used only when new pimo:Person is created
    //           in Nepomuk and in that case Nepomuk *always* signals propertyAdded
    //           with "groundingOccurrence", so we get the contacts either way.
    appendRow(new PersonsModelItem(res.uri()));
}

PersonsModelContactItem* PersonsModel::contactForIMAccount(const QUrl& uri) const
{
    QStandardItem* it = itemFromIndex(findRecursively(PersonsModel::IMAccountUriRole, uri));
    return dynamic_cast<PersonsModelContactItem*>(it);
}

void PersonsModel::createPersonFromContacts(const QList<QUrl>& contacts)
{
    Nepomuk2::SimpleResource newPimoPerson;
    newPimoPerson.addType(Nepomuk2::Vocabulary::PIMO::Person());

    foreach(const QUrl &contact, contacts) {
        newPimoPerson.addProperty(Nepomuk2::Vocabulary::PIMO::groundingOccurrence(), contact);
    }

    Nepomuk2::SimpleResourceGraph graph;
    graph << newPimoPerson;

    KJob *job = Nepomuk2::storeResources( graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties );
    connect(job, SIGNAL(finished(KJob*)), this, SLOT(jobFinished(KJob*)));
    //the new person will be added to the model by the resourceCreated and propertyAdded Nepomuk signals
}

void PersonsModel::removePerson(const QUrl &uri)
{
    Nepomuk2::Resource oldPerson(uri);
    oldPerson.remove();
}

void PersonsModel::removePersonFromModel(const QModelIndex &index)
{
    PersonsModelItem *person = dynamic_cast<PersonsModelItem*>(itemFromIndex(index));

    for (int i = 0; i < person->rowCount(); i++) {
        //reparent the contacts to toplevel
        invisibleRootItem()->appendRow(person->takeChild(i));
    }

    removeRow(index.row());
}

NepomukTpChannelDelegate* PersonsModel::tpChannelDelegate() const
{
    return d_ptr->ktpDelegate;
}
