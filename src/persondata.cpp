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

#include "persondata.h"
#include "persons-model.h"
#include <Nepomuk2/Resource>
#include <Nepomuk2/Query/Query>
#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Variant>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

struct PersonDataPrivate {
    PersonDataPrivate() : model(0) {}

    PersonsModel* model;
    QString id;
    QUrl uri;
};

PersonData::PersonData(QObject* parent)
    : QObject(parent)
    , d_ptr(new PersonDataPrivate)
{}

void PersonData::setContactId(const QString& id)
{
    Q_D(PersonData);
    if(d->id==id)
        return;

    //it should be basically the same query as in the persons model
    //only that here we're restricting it to a person
    QString query = QString::fromUtf8(
            "select ?uri ?pimo_groundingOccurrence ?nco_hasIMAccount"
                "?nco_imNickname ?telepathy_statusType ?nco_imID ?nco_imAccountType ?nco_hasEmailAddress"
                "?nco_imStatus ?nie_url "

                "WHERE { "
                    "?uri a nco:PersonContact. "
                    "?pimo_groundingOccurrence  pimo:groundingOccurrence     ?uri. "
                    "?uri                       nco:hasContactMedium         ?x. "
                    "?x                         ?y                           \"%1\"^^xsd:string. "

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
            "}").arg(id);

    delete d->model;
    d->model = new PersonsModel(this, true, query);
    connect(d->model, SIGNAL(peopleAdded()), SLOT(personInitialized()));
    connect(d->model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SIGNAL(dataChanged()));
}

void PersonData::setContactUri(const QUrl &uri)
{
    Q_D(PersonData);
    if(d->uri == uri)
        return;

    bool isContact = false;
    bool isPerson = false;

    QString query = QString::fromLatin1("select ?type WHERE { <%1> a ?type. }").arg(uri.toString());

    Soprano::QueryResultIterator it = Nepomuk2::ResourceManager::instance()->mainModel()->executeQuery( query, Soprano::Query::QueryLanguageSparql );
    while( it.next() ) {
        kDebug() << it["type"];
        if (it["type"].toString() == Nepomuk2::Vocabulary::PIMO::Person().toString()) {
            isPerson = true;
        }
        if (it["type"].toString() == Nepomuk2::Vocabulary::NCO::PersonContact().toString()) {
            isContact = true;
        }
    }

    if (isContact) {
        query = QString::fromUtf8(
            "select ?uri ?pimo_groundingOccurrence ?nco_hasIMAccount"
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
            "FILTER (?uri = <%1>)"
            "}").arg(uri.toString());
    } else if (isPerson) {
        query = QString::fromUtf8(
            "select ?uri ?pimo_groundingOccurrence ?nco_hasIMAccount"
            "?nco_imNickname ?telepathy_statusType ?nco_imID ?nco_imAccountType ?nco_hasEmailAddress"
            "?nco_imStatus ?nie_url "

            "WHERE { "
            "?uri a nco:PersonContact. "
            "?pimo_groundingOccurrence pimo:groundingOccurrence     ?uri. "

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
            "FILTER (?pimo_groundingOccurrence = <%1>)"
            "}").arg(uri.toString());
    }

        delete d->model;
        d->model = new PersonsModel(this, true, query);
        connect(d->model, SIGNAL(peopleAdded()), SLOT(personInitialized()));
        connect(d->model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SIGNAL(dataChanged()));
}


void PersonData::personInitialized()
{
    Q_D(PersonData);
    Q_ASSERT(d->model->rowCount()>0);
    emit dataInitialized();
}

QString PersonData::contactId() const
{
    Q_D(const PersonData);
    return d->id;
}

QUrl PersonData::uri() const
{
    return personIndex().data(PersonsModel::UriRole).toString();
}

QString PersonData::status() const
{
    return personIndex().data(PersonsModel::StatusRole).toString();
}

QUrl PersonData::avatar() const
{
    return personIndex().data(PersonsModel::PhotoRole).toUrl();
}

QString PersonData::nickname() const
{
    return personIndex().data(PersonsModel::NickRole).toString();
}

QStringList PersonData::contacts() const
{
    return personIndex().data(PersonsModel::ContactIdRole).toStringList();
}

QModelIndex PersonData::personIndex() const
{
    Q_D(const PersonData);
    Q_ASSERT(d->model->rowCount()>0);
    QModelIndex idx = d->model->index(0,0);
    Q_ASSERT(idx.isValid());
    return idx;
}
