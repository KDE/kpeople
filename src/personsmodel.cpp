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

#include "personsmodel.h"
#include "personitem.h"
#include "contactitem.h"
#include "resourcewatcherservice.h"
#include "personsmodelfeature.h"
#include "duplicatesfinder.h"
#include "basepersonsdatasource.h"
#include "datasourcewatcher.h"

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
#include <KServiceTypeTrader>

static int s_personId = 0;

struct PersonsModelPrivate {
    QString prepareQuery(const QUrl &uri = QUrl());

    QHash<QUrl, ContactItem*> contacts; //all contacts in the model
    QHash<QUrl, PersonItem*> persons;   //all persons
    QList<QUrl> pimoOccurances;         //contacts that are groundingOccurrences of persons
    QHash<QString, int> bindingRoleMap;
    DataSourceWatcher *dataSourceWatcher;

    PersonsModel::Features mandatoryFeatures;
    PersonsModel::Features optionalFeatures;

    QList<PersonsModelFeature> modelFeatures;

    BasePersonsDataSource imPlugin;

};

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

PersonsModel::PersonsModel(PersonsModel::Features mandatoryFeatures,
                           PersonsModel::Features optionalFeatures,
                           QObject *parent)
    : QStandardItemModel(parent)
    , d_ptr(new PersonsModelPrivate)
{
    Q_D(PersonsModel);
    d->dataSourceWatcher = new DataSourceWatcher(this);
    connect(d->dataSourceWatcher, SIGNAL(contactChanged(QUrl)), SLOT(contactChanged(QUrl)));

    QHash<int, QByteArray> names = roleNames();
    names.insert(PersonsModel::EmailsRole, "emails");
    names.insert(PersonsModel::PhonesRole, "phones");
    names.insert(PersonsModel::ContactIdRole, "contactId");
    names.insert(PersonsModel::IMsRole, "ims");
    names.insert(PersonsModel::NicknamesRole, "nicks");
    names.insert(PersonsModel::UriRole, "uri");
    names.insert(PersonsModel::FullNamesRole, "names");
    names.insert(PersonsModel::PhotosRole, "photos");
    setRoleNames(names);

    if (mandatoryFeatures != 0 || optionalFeatures != 0) {
        //this starts the query and populates the model
        setQueryFlags(mandatoryFeatures, optionalFeatures);
    }

    new ResourceWatcherService(this);
}

PersonsModel::~PersonsModel()
{
    delete d_ptr;
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

QList<PersonsModelFeature> PersonsModel::init(PersonsModel::Features mandatoryFeatures, PersonsModel::Features optionalFeatures)
{
    QList<PersonsModelFeature> features;

    //do nothing if empty flags are passed
    if (mandatoryFeatures == 0 && optionalFeatures == 0) {
        kWarning() << "null query flags passed!";
        return features;
    }

    //FIXME there is a bug in which a feature is in both mandatory and optional it is included twice.

    if (mandatoryFeatures & PersonsModel::FeatureIM) {
        kDebug() << "Adding mandatory IM";
        features << PersonsModelFeature::imModelFeature(false);
    }

    if (mandatoryFeatures & PersonsModel::FeatureAvatars) {
        kDebug() << "Adding mandatory Avatars";
        features << PersonsModelFeature::avatarModelFeature(false);
    }

    if (mandatoryFeatures & PersonsModel::FeatureGroups) {
        kDebug() << "Adding mandatory Groups";
        features << PersonsModelFeature::groupsModelFeature(false);
    }

    if (mandatoryFeatures & PersonsModel::FeatureEmails) {
        kDebug() << "Adding mandatory Emails";
        features << PersonsModelFeature::emailModelFeature(false);
    }

    if (mandatoryFeatures & PersonsModel::FeatureFullName) {
        kDebug() << "Adding mandatory FullName";
        features << PersonsModelFeature::fullNameModelFeature(false);
    }

    if (optionalFeatures & PersonsModel::FeatureIM) {
        kDebug() << "Adding optional IM";
        features << PersonsModelFeature::imModelFeature(true);
    }

    if (optionalFeatures & PersonsModel::FeatureAvatars) {
        kDebug() << "Adding optional Avatars";
        features << PersonsModelFeature::avatarModelFeature(true);
    }

    if (optionalFeatures & PersonsModel::FeatureGroups) {
        kDebug() << "Adding optional Groups";
        features << PersonsModelFeature::groupsModelFeature(true);
    }

    if (optionalFeatures & PersonsModel::FeatureEmails) {
        kDebug() << "Adding optional Emails";
        features << PersonsModelFeature::emailModelFeature(true);
    }

    if (optionalFeatures & PersonsModel::FeatureFullName) {
        kDebug() << "Adding optional FullName";
        features << PersonsModelFeature::fullNameModelFeature(true);
    }

    return features;
}

void PersonsModel::setQueryFlags(PersonsModel::Features mandatoryFeatures, PersonsModel::Features optionalFeatures)
{
    Q_D(PersonsModel);

    if (rowCount() > 0) {
        kWarning() << "Model is already populated";
        return;
    }

    if (d->mandatoryFeatures != mandatoryFeatures || d->optionalFeatures != optionalFeatures) {
        d->mandatoryFeatures = mandatoryFeatures;
        d->optionalFeatures = optionalFeatures;

        d->modelFeatures = init(mandatoryFeatures, optionalFeatures);

        Q_FOREACH(PersonsModelFeature feature, d->modelFeatures) {
            d->bindingRoleMap.unite(feature.bindingsMap());
        }
    }

//         d->bindingRoleMap.insert("nao_prefLabel", LabelRole);

    QString queryString = d->prepareQuery();

    QMetaObject::invokeMethod(this, "query", Qt::QueuedConnection, Q_ARG(QString, queryString));
}

void PersonsModel::query(const QString &queryString)
{
    qDebug() << queryString;
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
    const QUrl &currentUri = query->binding(QLatin1String("uri")).uri();

    //before any further processing, check if the current contact
    //is not a groundingOccurrence of a nepomuk:/me person, if yes, don't process it
    QUrl pimoPersonUri = query->binding(QLatin1String("pimo_groundingOccurrence")).uri();
    if (pimoPersonUri == QUrl(QLatin1String("nepomuk:/me"))) {
        query->next();
        return;
    } else if (pimoPersonUri.isEmpty()) {
        //if the person uri is empty, we need to fake one
        pimoPersonUri = QUrl("fakeperson:/" + QString::number(s_personId++));
    }

    //see if we don't have this contact already
    ContactItem *contactNode = d->contacts.value(currentUri);

    bool newContact = !contactNode;

    if (newContact) {
        contactNode = new ContactItem(currentUri);
        d->contacts.insert(currentUri, contactNode);
    } else {
        //TODO see if this actually happens. If not, we can remove the lookup above and optimise.
        //If it still does occur, with a good reason, remove this TODO
        kWarning() << "the same contact has appeared twice in results. Ignoring";
        query->next();
        return;
    }

    //iterate over the results and add the wanted properties into the contact
    foreach (const QString &bName, query->bindingNames()) {
        QHash<QString, int>::const_iterator it = d->bindingRoleMap.constFind(bName);
        if (it == d->bindingRoleMap.constEnd()) {
            continue;
        }

        Soprano::Node node = query->binding(bName);
        if (!node.isEmpty()) {
            //contact info fields are stored as lists seperated by ;;;
            //contact item can only take single values in addContactData so looping is done here
            foreach (const QString &data, node.toString().split(";;;")) {
                contactNode->addContactData(*it, data);
            }
        }
    }

    //if we have IM features watch for data changes
    if ((d->optionalFeatures | d->mandatoryFeatures) & FeatureIM) {
        const QString &imContactId = query->binding("nco_imID").toString();
        if (!imContactId.isEmpty())  {
            d->dataSourceWatcher->watchContact(imContactId, currentUri);
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

QPair<PersonsModel::Features, PersonsModel::Features> PersonsModel::queryFlags() const
{
    Q_D(const PersonsModel);
    return qMakePair(d->mandatoryFeatures, d->optionalFeatures);
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
        return item->index();
    }
    item = d->persons[uri];
    if (item) {
        return item->index();
    }
    return QModelIndex();
}

QList<QModelIndex> PersonsModel::indexesForUris(const QVariantList& uris) const
{
    Q_ASSERT(!uris.isEmpty());
    QList<QModelIndex> ret;
    foreach(const QVariant& uri, uris) {
        ret += indexForUri(uri.toUrl());
    }
    return ret;
}

void PersonsModel::addPerson(const Nepomuk2::Resource &res)
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

void PersonsModel::addContact(const Nepomuk2::Resource &res)
{
    Q_D(PersonsModel);
    ContactItem *item = new ContactItem(res.uri());
    item->loadData();
    d->dataSourceWatcher->watchContact(item->data(PersonsModel::IMsRole).toString(),
                                       item->data(PersonsModel::UriRole).toString());
    appendRow(item);
}

void PersonsModel::updateContact(ContactItem *contact)
{
    Q_D(PersonsModel);

    kDebug() << "Updating contact" << contact->uri();

    QString queryString = d->prepareQuery(contact->uri());

    Soprano::Model *m = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::Util::AsyncQuery *query = Soprano::Util::AsyncQuery::executeQuery(m, queryString, Soprano::Query::QueryLanguageSparql);
    query->setProperty("contactItem", QVariant::fromValue<ContactItem*>(contact));

    connect(query, SIGNAL(nextReady(Soprano::Util::AsyncQuery*)),
            this, SLOT(updateContactNextReady(Soprano::Util::AsyncQuery*)));

    connect(query, SIGNAL(finished(Soprano::Util::AsyncQuery*)),
            this, SLOT(updateContactFinished(Soprano::Util::AsyncQuery*)));
}

void PersonsModel::updateContactNextReady(Soprano::Util::AsyncQuery *query)
{
    Q_D(PersonsModel);

    ContactItem *contact = query->property("contactItem").value<ContactItem*>();

    if (!contact) {
        query->next();
        return;
    }

    foreach (const QString &bName, query->bindingNames()) {
        if (!d->bindingRoleMap.contains(bName))
            continue;

        int role = d->bindingRoleMap.value(bName);
        QString value = query->binding(bName).toString();
        if (!value.isEmpty()) {
            contact->addContactData(role, value);
        }
    }

    query->next();
}

void PersonsModel::updateContactFinished(Soprano::Util::AsyncQuery *query)
{
    ContactItem *contact = query->property("contactItem").value<ContactItem*>();

    if (!contact) {
        return;
    }

    contact->finishLoadingData();
}

ContactItem* PersonsModel::contactForIMAccount(const QUrl &uri) const
{
//     QStandardItem *it = itemFromIndex(findRecursively(PersonsModel::IMAccountUriRole, uri));
//     return dynamic_cast<ContactItem*>(it);
    return 0;
}

void PersonsModel::createPersonFromContacts(const QList<QUrl> &contacts)
{
    Q_ASSERT(!contacts.isEmpty());
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
    Q_ASSERT(!indexes.isEmpty());
    QList<QUrl> personUris;
    QList<QUrl> contactUris;

    Q_FOREACH(const QModelIndex &index, indexes) {

        if (index.data(PersonsModel::UriRole).toString().left(10).compare("fakeperson") == 0) {
            contactUris.append(index.data(PersonsModel::ChildContactUriRole).toUrl());
        } else {
            personUris.append(index.data(PersonsModel::UriRole).toUrl());
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
        removePerson(personUri);
    }
}

void PersonsModel::removePerson(const QUrl &uri)
{
    Nepomuk2::Resource oldPerson(uri);
    oldPerson.remove();
}

void PersonsModel::removePersonFromModel(const QModelIndex &index)
{
    kDebug() << "Removing person from model";
    PersonItem *person = dynamic_cast<PersonItem*>(itemFromIndex(index));
    if (!person) {
        kDebug() << "Invalid person, returning";
        return;
    }

    while (person->rowCount()) {
        invisibleRootItem()->appendRow(person->takeRow(person->rowCount() - 1));
    }

    removeRow(index.row());
}

void PersonsModel::contactChanged(const QUrl& uri)
{
    const QModelIndex index = indexForUri(uri);
    dataChanged(index, index); //FIXME it's normally bad to do this on a QStandardItemModel
}
