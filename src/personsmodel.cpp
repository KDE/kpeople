/*
    Persons Model
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

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

#include "personsmodel.h"
#include "personitem_p.h"
#include "contactitem_p.h"
#include "resourcewatcherservice_p.h"
#include "personsmodelfeature.h"
#include "duplicatesfinder_p.h"
#include "basepersonsdatasource.h"
#include "datasourcewatcher_p.h"

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
#include <kglobal.h>

static const KCatalogLoader loader("libkpeople");

using namespace KPeople;

namespace KPeople{
struct PersonsModelPrivate {
    QString prepareQuery(const QUrl &uri = QUrl());

    QHash<QUrl, ContactItem*> contacts; //all contacts in the model
    QHash<QUrl, PersonItem*> persons;   //all persons
    QHash<QString, int> bindingRoleMap;
    DataSourceWatcher *dataSourceWatcher;

    QList<PersonsModelFeature> modelFeatures;

    int fakePersonsCounter; //used to set fake persons uri in form of fakeperson:/N
};
}

QString PersonsModelPrivate::prepareQuery(const QUrl &uri)
{
    QString selectPart(QLatin1String("select DISTINCT ?uri ?pimo_groundingOccurrence "));
    QString queryPart(QLatin1String("WHERE { ?uri a nco:PersonContact. "
    "OPTIONAL { ?pimo_groundingOccurrence pimo:groundingOccurrence ?uri. } "
    "OPTIONAL { ?uri nao:prefLabel ?nao_prefLabel. } "));

    Q_FOREACH(PersonsModelFeature feature, modelFeatures) {
        queryPart.append(feature.queryPart());
        queryPart.append(" ");
    }

    Q_FOREACH(const QString &queryVariable, bindingRoleMap.keys()) {
        //Normally a contact with multiple email addresses (for example) will appear as multiple rows in the query
        //By using the sql group_digest function we group all values into a single list on the server rather than multiple rows. Values are separated by ";;;"

        //arg 2 is the seperator, 3 is the max length and 4 is a flag 1 meaning remove duplicates
        //5000 is an abitrary big number
        selectPart.append(QString("(sql:group_digest(?%1,';;;',5000,1) as ?%1) ").arg(queryVariable));
    }

    if (!uri.isEmpty()) {
        queryPart.replace("?uri", Soprano::Node::resourceToN3(uri));
    } else {
        queryPart.append("FILTER(?uri!=<nepomuk:/me>).");
    }

    selectPart.append(queryPart.append("}"));

    return selectPart;
}

//-----------------------------------------------------------------------------

PersonsModel::PersonsModel(QObject *parent)
    : QStandardItemModel(parent)
    , d_ptr(new PersonsModelPrivate)
{
    Q_D(PersonsModel);
    d->dataSourceWatcher = new DataSourceWatcher(this);
    connect(d->dataSourceWatcher, SIGNAL(contactChanged(QUrl)), SLOT(contactChanged(QUrl)));

    QHash<int, QByteArray> names = roleNames();
    names.insert(PersonsModel::EmailsRole, "emails");
    names.insert(PersonsModel::PhonesRole, "phones");
    names.insert(PersonsModel::IMsRole, "ims");
    names.insert(PersonsModel::NicknamesRole, "nicks");
    names.insert(PersonsModel::UriRole, "uri");
    names.insert(PersonsModel::FullNamesRole, "names");
    names.insert(PersonsModel::PhotosRole, "photos");
    setRoleNames(names);

    new ResourceWatcherService(this);
    d->fakePersonsCounter = 0;
}

PersonsModel::~PersonsModel()
{
    delete d_ptr;
}

template <class T>
QList<QStandardItem*> toStandardItems(const QList<T*> &items)
{
    QList<QStandardItem*> ret;
    ret.reserve(items.size());
    Q_FOREACH (QStandardItem *it, items) {
        ret += it;
    }
    return ret;
}

void PersonsModel::startQuery(const QList< PersonsModelFeature> &features)
{
    Q_D(PersonsModel);
    d->modelFeatures = features;
    Q_FOREACH(PersonsModelFeature feature, d->modelFeatures) {
        d->bindingRoleMap.unite(feature.bindingsMap());
    }

    QString queryString = d->prepareQuery();
    QMetaObject::invokeMethod(this, "query", Qt::QueuedConnection, Q_ARG(QString, queryString));
}


void PersonsModel::query(const QString &queryString)
{
    kDebug() << queryString;
    Q_ASSERT(rowCount() == 0);

    Soprano::Model *m = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::Util::AsyncQuery *query = Soprano::Util::AsyncQuery::executeQuery(m, queryString, Soprano::Query::QueryLanguageSparql);

    connect(query, SIGNAL(nextReady(Soprano::Util::AsyncQuery*)),
            this, SLOT(nextReady(Soprano::Util::AsyncQuery*)));

    connect(query, SIGNAL(finished(Soprano::Util::AsyncQuery*)),
            this, SLOT(queryFinished(Soprano::Util::AsyncQuery*)));
}

void PersonsModel::nextReady(Soprano::Util::AsyncQuery *query)
{
    Q_D(PersonsModel);
    //if we're doing update, the contact uri is passed as property of the query
    QUrl updateUri = query->property("contactUri").toUrl();
    ContactItem *contactNode = d->contacts.value(updateUri);
    bool newContact = !contactNode;
    QUrl currentUri;

    if (newContact) {
        currentUri = query->binding(QLatin1String("uri")).uri();
        contactNode = new ContactItem(currentUri);
        d->contacts.insert(currentUri, contactNode);
    } else {
        currentUri = updateUri;
    }

    //iterate over the results and add the wanted properties into the contact
    Q_FOREACH (const QString &bName, query->bindingNames()) {
        QHash<QString, int>::const_iterator it = d->bindingRoleMap.constFind(bName);
        if (it == d->bindingRoleMap.constEnd()) {
            continue;
        }

        Soprano::Node node = query->binding(bName);
        if (!node.isEmpty()) {
            //contact info fields are stored as lists seperated by ;;;
            //contact item can only take single values in addContactData so looping is done here
            Q_FOREACH (const QString &data, node.toString().split(";;;")) {
                contactNode->addContactData(*it, data);
            }
        }
    }

    //if we have IM features watch for data changes
    const QString &imContactId = query->binding("nco_imID").toString();
    if (!imContactId.isEmpty())  {
        d->dataSourceWatcher->watchContact(imContactId, currentUri);
    }

    QUrl pimoPersonUri = query->binding(QLatin1String("pimo_groundingOccurrence")).uri();

    if (newContact) {
        if (pimoPersonUri.isEmpty()) {
            //if the person uri is empty, we need to fake one
            pimoPersonUri = QUrl("fakeperson:/" + QString::number(d->fakePersonsCounter++));
        }
        //look for existing person items
        QHash< QUrl, PersonItem* >::const_iterator pos = d->persons.constFind(pimoPersonUri);
        if (pos == d->persons.constEnd()) {
            //this means no person exists yet, so lets create new one
            pos = d->persons.insert(pimoPersonUri, new PersonItem(pimoPersonUri));
        }

        pos.value()->appendRow(contactNode);
    }

    query->next();
}

void PersonsModel::queryFinished(Soprano::Util::AsyncQuery *query)
{
    Q_UNUSED(query)
    Q_D(PersonsModel);
    //add the persons to the model
    if (!d->persons.isEmpty()) {
        invisibleRootItem()->appendRows(toStandardItems(d->persons.values()));
    }

    emit modelInitialized();
    kDebug() << "Model ready";
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

QList<PersonsModelFeature> PersonsModel::modelFeatures() const
{
    Q_D(const PersonsModel);
    return d->modelFeatures;
}

QModelIndex PersonsModel::indexForUri(const QUrl &uri) const
{
    Q_D(const PersonsModel);

    QStandardItem *item = 0;
    //The URI could be either a URI for a person, or a URI for a contact
    //therefore we search both hashes and return the index if it exists
    item = d->contacts[uri];
    if (item) {
        //item->index() returns invalid index for some reason
        return indexFromItem(item);
    }
    item = d->persons[uri];
    if (item) {
        //item->index() returns invalid index for some reason
        return indexFromItem(item);
    }
    return QModelIndex();
}

QList<QModelIndex> PersonsModel::indexesForUris(const QVariantList& uris) const
{
    Q_ASSERT(!uris.isEmpty());
    QList<QModelIndex> ret;
    Q_FOREACH (const QVariant& uri, uris) {
        ret += indexForUri(uri.toUrl());
    }
    return ret;
}

ContactItem* PersonsModel::contactItemForUri(const QUrl &uri) const
{
    Q_D(const PersonsModel);
    return d->contacts.value(uri);
}

PersonItem* PersonsModel::personItemForUri(const QUrl &uri) const
{
    Q_D(const PersonsModel);
    return d->persons.value(uri);
}

void PersonsModel::contactChanged(const QUrl &uri)
{
    const QModelIndex index = indexForUri(uri);
    if (index.isValid()) {
        dataChanged(index, index); //FIXME it's normally bad to do this on a QStandardItemModel
        if (index.parent().isValid()) {
            dataChanged(index.parent(), index.parent());
        }
    }
}

void PersonsModel::addContact(const QUrl &uri)
{
    Q_D(PersonsModel);
    ContactItem *item = new ContactItem(uri);
    d->dataSourceWatcher->watchContact(item->data(PersonsModel::IMsRole).toString(),
                                       item->data(PersonsModel::UriRole).toString());

    //create new fake person for this contact
    PersonItem *person = new PersonItem(QUrl("fakeperson:/" + QString::number(d->fakePersonsCounter++)));

    d->contacts.insert(uri, item);
    d->persons.insert(person->uri(), person);

    //append the ContactItem to the fake person PersonItem
    person->appendRow(item);
    appendRow(person);

    //load the contact data
    item->loadData(this);
}

void PersonsModel::updateContact(ContactItem *contact)
{
    if (contact) {
        updateContact(contact->uri());
    }
}

void PersonsModel::updateContact(const QUrl &uri)
{
    Q_D(PersonsModel);

    kDebug() << "Updating contact" << uri;

    ContactItem *contact = d->contacts[uri];

    if (!contact) {
        kWarning() << "Contact not found! Uri is" << uri;
        return;
    }

    contact->clear();

    QString queryString = d->prepareQuery(uri);

    Soprano::Model *m = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::Util::AsyncQuery *query = Soprano::Util::AsyncQuery::executeQuery(m, queryString, Soprano::Query::QueryLanguageSparql);
    query->setProperty("contactUri", QVariant(uri));


    connect(query, SIGNAL(nextReady(Soprano::Util::AsyncQuery*)),
            this, SLOT(nextReady(Soprano::Util::AsyncQuery*)));

    connect(query, SIGNAL(finished(Soprano::Util::AsyncQuery*)),
            this, SLOT(updateContactFinished(Soprano::Util::AsyncQuery*)));
}

void PersonsModel::updateContactFinished(Soprano::Util::AsyncQuery *query)
{
    Q_D(const PersonsModel);
    QUrl contactUri = query->property("contactUri").toUrl();

    ContactItem *contact = d->contacts[contactUri];

    if (!contact) {
        return;
    }

    contact->finishLoadingData();
}

void PersonsModel::removeContact(const QUrl &uri)
{
    Q_D(PersonsModel);

    ContactItem *contact = d->contacts.value(uri);

    if (!contact) {
        kWarning() << "Contact not found! Uri is" << uri;
        return;
    }

    PersonItem *person = dynamic_cast<PersonItem*>(contact->parent());

    if (!person) {
        kWarning() << "Found contact without valid person!";
        return;
    }

    person->removeRow(contact->row());
    d->contacts.remove(uri);

    //if the person is now empty, remove it from the model too
    if (person->rowCount() == 0) {
        removePerson(person->uri());
    } else {
        person->contactDataChanged();
    }

}

PersonItem *PersonsModel::addPerson(const QUrl &uri)
{
    Q_D(PersonsModel);

    PersonItem *newPerson = new PersonItem(uri);
    d->persons.insert(uri, newPerson);
    appendRow(newPerson);
    return newPerson;
}

void PersonsModel::removePerson(const QUrl &uri)
{
    Q_D(PersonsModel);

    PersonItem *person = d->persons.value(uri);

    if (!person) {
        kWarning() << "Person not found! Uri is" << uri;
        return;
    }

    //move the child contacts into new fake persons

    //when called from addContactToPerson if URI is a fakeperson the child has already been removed
    while (person->rowCount()) {
        QUrl pimoPersonUri = QUrl("fakeperson:/" + QString::number(d->fakePersonsCounter++));
        PersonItem* p = addPerson(pimoPersonUri);
        p->appendRow(person->takeRow(0));
    }

    invisibleRootItem()->removeRow(person->row());
    d->persons.remove(uri);
}


void PersonsModel::addContactsToPerson(const QUrl &personUri, const QList<QUrl> &_contacts)
{
    Q_D(PersonsModel);

    PersonItem *person = d->persons.value(personUri);

    if (!person) {
        kWarning() << "Person not found! Uri is" << personUri;
        return;
    }

    QList<QUrl> contacts(_contacts);

    //get existing child-contacts and remove them from the new ones
    QVariantList uris = person->data(PersonsModel::ChildContactsUriRole).toList();
    Q_FOREACH (const QVariant &uri, uris) {
        contacts.removeOne(uri.toUrl());
    }

    //query the model for the contacts, if they are present, then need to be just moved
    QList<QStandardItem*> personContacts;
    Q_FOREACH (const QUrl &uri, contacts) {
        ContactItem *contact = d->contacts.value(uri);

        if (!contact) {
            kDebug() << "Contact not found" << uri;
            continue;
        }

        PersonItem *parentPerson = dynamic_cast<PersonItem*>(contact->parent());

        if (!parentPerson) {
            kWarning() << "Found contact without valid person!";
        }
        Q_ASSERT(parentPerson); //this should never ever happen

        if (parentPerson->uri().scheme() == "fakeperson") {
            //the contact does not have any real person
            parentPerson->takeRow(0);
            //remove the fake person
            removePerson(parentPerson->uri());
        } else {
            //the contact does already have a person so we just append
            //it without removing from the original person
        }
        personContacts.append(contact);
    }

    //append the moved contacts to this person and remove them from 'contacts'
    //so they are not added twice
    QList<QStandardItem*> newContacts;
    Q_FOREACH (QStandardItem *contactItem, personContacts) {
        ContactItem *contact = dynamic_cast<ContactItem*>(contactItem);
        newContacts += contact;
        contacts.removeOne(contact->uri());
    }

    //if we have any contacts left, we need to create them
    Q_FOREACH (const QUrl &uri, contacts) {
        ContactItem *item = new ContactItem(uri);
        d->dataSourceWatcher->watchContact(item->data(PersonsModel::IMsRole).toString(),
                                           item->data(PersonsModel::UriRole).toString());

        d->contacts.insert(uri, item);

        //load the contact data
        item->loadData(this);

        newContacts += item;
    }
    person->appendRows(newContacts);
    person->contactDataChanged();
}


void PersonsModel::removeContactsFromPerson(const QUrl &personUri, const QList<QUrl> &contacts)
{
//     Q_D(PersonsModel);
    Q_UNUSED(personUri);

    //simply remove the contacts from the model and re-add them
    //this will take care of removing empty persons in case the contact
    //is the last one left and also creating new fake persons
    //without duplicating any code, plus the contact data gets
    //updated in the process
    Q_FOREACH (const QUrl &contactUri, contacts) {
        removeContact(contactUri);
        addContact(contactUri);
    }
}

class MergePersonsAndContacts : public KJob
{
    Q_OBJECT
    public:
        MergePersonsAndContacts(const QList<QUrl>& personUris, const QList<QUrl>& contactUris)
            : KJob()
            , m_personUris(personUris)
            , m_contactUris(contactUris)
        {
            m_contactUris << personUris.first();
        }

    virtual void start() {
        // will add contacts to one the persons
        KJob* job = PersonsModel::createPersonFromUris(m_contactUris) ;
        connect(job, SIGNAL(finished(KJob*)), SLOT(contactsMerged()));
    }

    public slots:
        void contactsMerged()
        {
            //we merge all the persons
            KJob* jobB = PersonsModel::createPersonFromUris(m_personUris);
            connect(jobB, SIGNAL(finished(KJob*)), this, SLOT(jobFinished(KJob*)));
        }

    private:
        QList<QUrl> m_personUris;
        QList<QUrl> m_contactUris;
};

KJob* PersonsModel::createPersonFromUris(const QList<QUrl> &uris)
{
    Q_ASSERT(uris.size()>1);
    QList<QUrl> personUris;
    QList<QUrl> contactUris;

    //uris identification
    Soprano::Model *model = Nepomuk2::ResourceManager::instance()->mainModel();

    Q_FOREACH (const QUrl &uri, uris) {
        //we should never get the fakeperson:/ uri here
        Q_ASSERT(!uri.toString().startsWith("fakeperson"));
        //for each uri we query it as if it was pimo:Person
        QString query = QString::fromLatin1("SELECT DISTINCT ?r WHERE { %1 rdf:type pimo:Person }")
                        .arg(Soprano::Node::resourceToN3(uri));

        Soprano::QueryResultIterator it = model->executeQuery(query, Soprano::Query::QueryLanguageSparql);
        //if the resource with the given uri is in fact pimo:Person,
        //we get back one result, if it's not pimo:Person, it's nco:PersonContact
        if (it.allElements().size() == 1) {
            personUris << uri;
        } else {
            contactUris << uri;
        }
    }

    KJob *job = 0;

    if (personUris.isEmpty() && !contactUris.isEmpty()) {
        Nepomuk2::SimpleResource newPimoPerson;
        newPimoPerson.addType(Nepomuk2::Vocabulary::PIMO::Person());

        Q_FOREACH (const QUrl &contact, contactUris) {
            newPimoPerson.addProperty(Nepomuk2::Vocabulary::PIMO::groundingOccurrence(), contact);
        }

        Nepomuk2::SimpleResourceGraph graph;
        graph << newPimoPerson;

        job = Nepomuk2::storeResources( graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties );
        //the new person will be added to the model by the resourceCreated and propertyAdded Nepomuk signals
    } else if (personUris.size() == 1 && !contactUris.isEmpty()) {
        //put the contacts from QList<QUrl> into QVariantList
        QVariantList contactsList;
        Q_FOREACH (const QUrl &contact, contactUris) {
            contactsList << contact;
        }
        job = Nepomuk2::addProperty(QList<QUrl>() << personUris.first(),
                                          Nepomuk2::Vocabulary::PIMO::groundingOccurrence(),
                                          QVariantList() << contactsList);
    } else if (personUris.size() > 1 && contactUris.isEmpty()) {
        job = Nepomuk2::mergeResources(personUris);
    } else if (personUris.size() > 1 && !contactUris.isEmpty()) {
        job = new MergePersonsAndContacts(personUris, contactUris);
        job->start();
    } else {
        kWarning() << "not implemented yet";
    }

    //TODO
    //This makes the job synchronous, which is bad.. but there's already a synchronous call in this method anyway (line 591).
    //If Soprano is blocked we already block our UI so this has little real impact

    //we need some changes to make sure that if this method is called twice in quick succession for the same contacts
    //otherwise we can end up making attaching a single contact to two different PIMO:Person's as we check if the contact is owned by a person whilst a merge
    //job is happening asyncronously. This effectively breaks the database
    //This cannot be fixed without a _signifcant_ refactor.

    if (job) {
        job->exec();
    }

    return job;
}

void PersonsModel::unlinkContactFromPerson(const QUrl &personUri, const QList<QUrl> &contactUris)
{
    QVariantList contactsList;
    Q_FOREACH(const QUrl &contact, contactUris) {
        contactsList << contact;
    }

    KJob *job = Nepomuk2::removeProperty(QList<QUrl>() << personUri,
                                         Nepomuk2::Vocabulary::PIMO::groundingOccurrence(),
                                         QVariantList() << contactsList);
    job->exec();
    if (job->error()) {
        kWarning() << "Removing contacts from person failed:" << job->errorString();
        return;
    }

    Soprano::Model *model = Nepomuk2::ResourceManager::instance()->mainModel();

    QString query = QString::fromLatin1("select distinct ?go where { %1 pimo:groundingOccurrence ?go . }")
    .arg(Soprano::Node::resourceToN3(personUri));

    Soprano::QueryResultIterator it = model->executeQuery(query, Soprano::Query::QueryLanguageSparql);
    if (it.allBindings().count() == 1) {
        //remove the person from Nepomuk
        Nepomuk2::Resource oldPerson(personUri);
        oldPerson.remove();
    }
}

#include "personsmodel.moc"
#include "moc_personsmodel.cpp"
