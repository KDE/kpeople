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
    QString id;
    Nepomuk2::Resource resource;
    QList< Nepomuk2::Resource > contacts;
};

PersonData::PersonData(QObject* parent)
    : QObject(parent)
    , d_ptr(new PersonDataPrivate)
{}

void PersonData::setContactId(const QString& id)
{
    Q_D(PersonData);
    if(id==d->id)
        return;
    
    QString query = QString::fromUtf8(
        "select distinct ?uri where { "
            "?uri a pimo:Person. "
            "?uri pimo:groundingOccurrence ?u. "
            "?u nco:hasContactMedium ?x. "
            "?x ?y \"%1\"^^xsd:string. "
        "}").arg(id);

    Soprano::Model* m = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = m->executeQuery(query, Soprano::Query::QueryLanguageSparql);
    if(it.next()) {
        d->id = id;
        d->resource = Nepomuk2::Resource(it["uri"].uri());
        d->contacts = d->resource.property(Nepomuk2::Vocabulary::PIMO::groundingOccurrence()).toResourceList();
        emit contactChanged();
    }
}

QString PersonData::contactId() const
{
    Q_D(const PersonData);
    return d->id;
}

QUrl PersonData::uri() const
{
    Q_D(const PersonData);
    return d->resource.uri();
}

QString PersonData::status() const
{
    Q_D(const PersonData);
    foreach(const Nepomuk2::Resource& res, d->contacts) {
        if(res.hasProperty(Nepomuk2::Vocabulary::NCO::hasIMAccount())) {
            Nepomuk2::Resource imacc(res.property(Nepomuk2::Vocabulary::NCO::hasIMAccount()).toResource());
            if(imacc.hasProperty(Nepomuk2::Vocabulary::NCO::imStatus()))
                return imacc.property(Nepomuk2::Vocabulary::NCO::imStatus()).toString();
        }
    }
    return QString();
}

QUrl PersonData::avatar() const
{
    Q_D(const PersonData);
    foreach(const Nepomuk2::Resource& res, d->contacts) {
        Nepomuk2::Resource imacc(res.property(Nepomuk2::Vocabulary::NCO::photo()).toResource());
        if(imacc.hasProperty(Nepomuk2::Vocabulary::NIE::url()))
            return imacc.property(Nepomuk2::Vocabulary::NIE::url()).toUrl();
    }
    return QUrl();
}

QString PersonData::nickname() const
{
    Q_D(const PersonData);
    foreach(const Nepomuk2::Resource& res, d->contacts) {
        Nepomuk2::Resource imacc(res.property(Nepomuk2::Vocabulary::NCO::nickname()).toResource());
        if(imacc.hasProperty(Nepomuk2::Vocabulary::NCO::nickname()))
            return imacc.property(Nepomuk2::Vocabulary::NCO::nickname()).toString();
    }
    return QString();
}
