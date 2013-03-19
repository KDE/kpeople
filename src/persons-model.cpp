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
#include "person-item.h"
#include "contact-item.h"
#include "resource-watcher-service.h"

#include <Soprano/Query/QueryLanguage>
#include <Soprano/QueryResultIterator>
#include <Soprano/Model>
#include <Soprano/Util/AsyncQuery>
#include <Soprano/Vocabulary/NAO>

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
    QHash<QUrl, ContactItem*> contacts; //all contacts in the model
    QHash<QUrl, PersonItem*> persons;         //all persons
    QList<QUrl> pimoOccurances;                     //contacts that are groundingOccurrences of persons
    QHash<QString, PersonsModel::Role> bindingRoleMap;
};

PersonsModel::PersonsModel(QObject *parent, bool init, const QString &customQuery)
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
    names.insert(PersonsModel::ContactsCountRole, "contactsCount");
    names.insert(PersonsModel::ResourceTypeRole, "resourceType");
    setRoleNames(names);

    Q_D(PersonsModel);

    if (init) {
        QString nco_query = customQuery;
        if (customQuery.isEmpty()) {
            //TODO: Fetch Phone number?
            d->bindingRoleMap.insert("uri", UriRole);
            d->bindingRoleMap.insert("nco_imNickname", NickRole);
            d->bindingRoleMap.insert("nao_prefLabel", LabelRole);
            d->bindingRoleMap.insert("nco_imID", IMRole);
            d->bindingRoleMap.insert("nco_imImAcountType", IMAccountTypeRole);
            d->bindingRoleMap.insert("nco_hasIMAccount", IMAccountUriRole);
            d->bindingRoleMap.insert("nco_contactGroupName", ContactGroupsRole);
            d->bindingRoleMap.insert("nie_url", PhotoRole);
            d->bindingRoleMap.insert("nco_emailAddress", EmailRole);

            nco_query = QString::fromUtf8(
            "select DISTINCT ?uri ?pimo_groundingOccurrence ?nco_hasIMAccount "
            "?nco_imNickname ?nco_imID ?nco_imAccountType ?nco_hasEmailAddress "
            "?nie_url ?nao_prefLabel ?nco_contactGroupName "

                "WHERE { "
                    "?uri a nco:PersonContact. "

                    "OPTIONAL { ?pimo_groundingOccurrence  pimo:groundingOccurrence  ?uri. }"
                    "OPTIONAL { ?uri  nao:prefLabel  ?nao_prefLabel. }"

//                    "OPTIONAL { "
                        "?uri                     nco:hasIMAccount            ?nco_hasIMAccount. "
                        "OPTIONAL { ?nco_hasIMAccount          nco:imNickname              ?nco_imNickname. } "
//                         "OPTIONAL { ?nco_hasIMAccount          nco:isAccessedBy            ?nco_isAccessedBy . } "
//                         "OPTIONAL { ?nco_isAccessedBy          telepathy:accountIdentifier ?telepathy_accountIdentifier . } "
                        "OPTIONAL { ?nco_hasIMAccount          nco:imID                    ?nco_imID. } "
                        "OPTIONAL { ?nco_hasIMAccount          nco:imAccountType           ?nco_imAccountType. } "
//                    " } "
                    "OPTIONAL { "
                        "?uri nco:belongsToGroup ?nco_belongsToGroup . "
                        "?nco_belongsToGroup nco:contactGroupName ?nco_contactGroupName . "
                    " }"
                    "OPTIONAL {"
                        "?uri                       nco:photo                   ?phRes. "
                        "?phRes                     nie:url                     ?nie_url. "
                    " } "
                    "OPTIONAL { "
                        "?uri                       nco:hasEmailAddress         ?nco_hasEmailAddress. "
                        "?nco_hasEmailAddress       nco:emailAddress            ?nco_emailAddress. "
                    " } "
                    "FILTER(?uri!=<nepomuk:/me>). "
                "}");
        }

        QMetaObject::invokeMethod(this, "query", Qt::QueuedConnection, Q_ARG(QString, nco_query));
        new ResourceWatcherService(this);
    }
}

template <class T>
QList<QStandardItem*> toStandardItems(const QList<T*> &items)
{
    QList<QStandardItem*> ret;
    foreach (QStandardItem *it, items) {
        ret += it;
    }
    return ret;
}


void PersonsModel::query(const QString &nco_query)
{
    Q_ASSERT(rowCount() == 0);

    Soprano::Model *m = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::Util::AsyncQuery *query = Soprano::Util::AsyncQuery::executeQuery(m, nco_query, Soprano::Query::QueryLanguageSparql);

    connect(query, SIGNAL(nextReady(Soprano::Util::AsyncQuery*)),
            this, SLOT(nextReady(Soprano::Util::AsyncQuery*)));

    connect(query, SIGNAL(finished(Soprano::Util::AsyncQuery*)),
            this, SLOT(queryFinished(Soprano::Util::AsyncQuery*)));
}

void PersonsModel::nextReady(Soprano::Util::AsyncQuery *query)
{
    Q_D(PersonsModel);
    QUrl currentUri = query->binding(QLatin1String("uri")).uri();

    //before any further processing, check if the current contact
    //is not a groundingOccurrence of a nepomuk:/me person, if yes, don't process it
    QUrl pimoPersonUri = query->binding(QLatin1String("pimo_groundingOccurrence")).uri();
    if (pimoPersonUri == QUrl(QLatin1String("nepomuk:/me"))) {
        query->next();
        return;
    }

    //see if we don't have this contact already
    ContactItem *contactNode = d->contacts.value(currentUri);

    bool newContact = !contactNode;

    if (newContact) {
        contactNode = new ContactItem(currentUri);
        d->contacts.insert(currentUri, contactNode);
    } else {
        //FIXME: for some reason we get most of the contacts twice in the resultset,
        //       disabling duplicate processing for now to speedup loading time;
        //       also it turns out that the same values are always passed the second time
        //       so no point processing it again
        query->next();
        return;
    }

    //iterate over the results and add the wanted properties into the contact
    foreach (const QString &bName, query->bindingNames()) {
        if (!d->bindingRoleMap.contains(bName)) {
            continue;
        }

        Role role = d->bindingRoleMap.value(bName);
        QString value = query->binding(bName).toString();
        if (!value.isEmpty()) {
            contactNode->addData(role, value);
        }
    }

    if (!pimoPersonUri.isEmpty()) {
        //look for existing person items
        QHash< QUrl, PersonItem* >::const_iterator pos = d->persons.constFind(pimoPersonUri);
        if (pos == d->persons.constEnd()) {
            //this means no person exists yet, so lets create new one
            pos = d->persons.insert(pimoPersonUri, new PersonItem(pimoPersonUri));
        }
        //FIXME: we need to check if the contact is not already present in person's children,
        //       from testing however it turns out that checking newContact == true
        //       is enough
        if (newContact) {
            pos.value()->appendRow(contactNode);
            d->pimoOccurances.append(currentUri);
        }
    }

    query->next();
}

void PersonsModel::queryFinished(Soprano::Util::AsyncQuery *query)
{
    Q_UNUSED(query)
    Q_D(PersonsModel);
    //add the persons to the model
    if (!d->persons.values().isEmpty()) {
        invisibleRootItem()->appendRows(toStandardItems(d->persons.values()));
    }

    Q_FOREACH(const QUrl &uri, d->pimoOccurances) {
        //remove all contacts being groundingOccurrences
        d->contacts.remove(uri);
    }
    //add the remaining contacts to the model as top level items
    if (!d->contacts.values().isEmpty()) {
        invisibleRootItem()->appendRows(toStandardItems(d->contacts.values()));
    }
    emit peopleAdded();
    kDebug() << "Model ready";
}

void PersonsModel::unmerge(const QUrl &contactUri, const QUrl &personUri)
{
    Nepomuk2::Resource oldPerson(personUri);
    Q_ASSERT(oldPerson.property(Nepomuk2::Vocabulary::PIMO::groundingOccurrence()).toUrlList().size() >= 2 && "there's nothing to unmerge...");
    oldPerson.removeProperty(Nepomuk2::Vocabulary::PIMO::groundingOccurrence(), contactUri);
    if (!oldPerson.hasProperty(Nepomuk2::Vocabulary::PIMO::groundingOccurrence())) {
        oldPerson.remove();
    }

//     Nepomuk2::SimpleResource newPerson;
//     newPerson.addType( Nepomuk2::Vocabulary::PIMO::Person() );
//     newPerson.setProperty(Nepomuk2::Vocabulary::PIMO::groundingOccurrence(), contactUri);
//
//     Nepomuk2::SimpleResourceGraph graph;
//     graph << newPerson;
//
//     KJob * job = Nepomuk2::storeResources( graph );
//     job->setProperty("uri", contactUri);
//     job->setObjectName("Unmerge");
//     connect(job, SIGNAL(finished(KJob*)), SLOT(jobFinished(KJob*)));
//     job->start();
}

void PersonsModel::merge(const QList<QUrl> &persons)
{
    KJob *job = Nepomuk2::mergeResources(persons);
    job->setObjectName("Merge");
    connect(job, SIGNAL(finished(KJob*)), SLOT(jobFinished(KJob*)));
}

void PersonsModel::merge(const QVariantList &persons)
{
    QList<QUrl> conv;
    foreach (const QVariant &p, persons)
        conv += p.toUrl();
    merge(conv);
}

void PersonsModel::jobFinished(KJob *job)
{
    if (job->error()!=0) {
        kWarning() << job->objectName() << " failed for "<< job->property("uri").toString() << job->errorText() << job->errorString();
    } else {
        kWarning() << job->objectName() << " done: "<< job->property("uri").toString();
    }
}

QModelIndex PersonsModel::findRecursively(int role, const QVariant &value, const QModelIndex &idx) const
{
    if (idx.isValid() && data(idx, role) == value) {
        return idx;
    }
    int rows = rowCount(idx);
    for (int i = 0; i < rows; i++) {
        QModelIndex ret = findRecursively(role, value, index(i, 0, idx));
        if (ret.isValid()) {
            return ret;
        }
    }

    return QModelIndex();
}

QModelIndex PersonsModel::indexForUri(const QUrl &uri) const
{
    return findRecursively(PersonsModel::UriRole, uri);
}

//FIXME: rename this to addPerson
void PersonsModel::createPerson(const Nepomuk2::Resource &res)
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
    appendRow(new PersonItem(res.uri()));
}

//FIXME: rename this to addContact
void PersonsModel::createContact(const Nepomuk2::Resource &res)
{
    ContactItem *item = new ContactItem(res.uri());
    item->loadData();
    appendRow(item);
}

ContactItem* PersonsModel::contactForIMAccount(const QUrl &uri) const
{
    QStandardItem *it = itemFromIndex(findRecursively(PersonsModel::IMAccountUriRole, uri));
    return dynamic_cast<ContactItem*>(it);
}

void PersonsModel::createPersonFromContacts(const QList<QUrl> &contacts)
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

void PersonsModel::createPersonFromIndexes(const QList<QModelIndex> &indexes)
{
    QList<QUrl> personUris;
    QList<QUrl> contactUris;


    Q_FOREACH(const QModelIndex &index, indexes) {
        if (index.data(PersonsModel::ResourceTypeRole).toUInt() == PersonsModel::Person) {
            personUris.append(index.data(PersonsModel::UriRole).toUrl());
        } else {
            contactUris.append(index.data(PersonsModel::UriRole).toUrl());
        }
    }

    if (personUris.isEmpty()) {
        kDebug() << "Got only contacts, creating pimo:person";
        createPersonFromContacts(contactUris);
    } else if (personUris.size() == 1) {
        kDebug() << "Got one pimo:person, adding contacts to it";
        addContactsToPerson(personUris.first(), contactUris);
    } else {
        kDebug() << "Got two pimo:persons, unsupported for now";
    }
}

void PersonsModel::addContactsToPerson(const QUrl &personUri, const QList<QUrl> &contacts)
{
    //put the contacts from QList<QUrl> into QVariantList
    QVariantList contactsList;
    Q_FOREACH(const QUrl &contact, contacts) {
        contactsList << contact;
    }

    KJob *job = Nepomuk2::addProperty(QList<QUrl>() << personUri,
                                      Nepomuk2::Vocabulary::PIMO::groundingOccurrence(),
                                      QVariantList() << contactsList);
    connect(job, SIGNAL(finished(KJob*)), this, SLOT(jobFinished(KJob*)));
}

void PersonsModel::removeContactsFromPerson(const QUrl &personUri, const QList<QUrl> &contacts)
{
    QVariantList contactsList;
    Q_FOREACH(const QUrl &contact, contacts) {
        contactsList << contact;
    }

    KJob *job = Nepomuk2::removeProperty(QList<QUrl>() << personUri,
                                         Nepomuk2::Vocabulary::PIMO::groundingOccurrence(),
                                         QVariantList() << contactsList);
    connect(job, SIGNAL(finished(KJob*)), this, SLOT(jobFinished(KJob*)));
}

void PersonsModel::removePerson(const QUrl &uri)
{
    Nepomuk2::Resource oldPerson(uri);
    oldPerson.remove();
}

void PersonsModel::removePersonFromModel(const QModelIndex &index)
{
    PersonItem *person = dynamic_cast<PersonItem*>(itemFromIndex(index));
    for (int i = 0; i < person->rowCount(); ) {
        //reparent the contacts to toplevel
        invisibleRootItem()->appendRow(person->takeRow(i));
    }

    removeRow(index.row());
}
