/*
    KPeople
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>
    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>

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
#include "persons-model-item.h"
#include "persons-presence-model.h"

#include <Nepomuk2/Resource>
#include <Nepomuk2/Query/Query>
#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/ResourceWatcher>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Variant>

#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include <KDebug>

using namespace Nepomuk2::Vocabulary;

class PersonDataPrivate {
public:
    QString uri;
    QString id;
    Nepomuk2::Resource res;
    PersonsPresenceModel *presenceModel;
};

K_GLOBAL_STATIC(PersonsPresenceModel, s_presenceModel)

PersonData::PersonData(const QString &uri, QObject *parent)
    : QObject(parent),
      d_ptr(new PersonDataPrivate)
{
    Q_D(PersonData);
    d->uri = uri;
    d->res = Nepomuk2::Resource(uri);

    d->res.setWatchEnabled(true);
    d->presenceModel = s_presenceModel;

    connect(d->presenceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SIGNAL(dataChanged()));

    Nepomuk2::ResourceWatcher *watcher = new Nepomuk2::ResourceWatcher(this);
    watcher->addResource(d->res);

    connect(watcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SIGNAL(dataChanged()));
}

void PersonData::setContactId(const QString &id)
{
    Q_D(PersonData);
    if (d->id == id) {
        return;
    }

    d->id = id;

    QString query = QString::fromUtf8(
        "select DISTINCT ?uri "
        "WHERE { "
            "?uri a nco:PersonContact. "
            "?uri nco:hasContactMedium ?a . "
            "?a ?b \"%1\"^^xsd:string . "
        "}").arg(id);

    Soprano::Model *model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery(query, Soprano::Query::QueryLanguageSparqlNoInference);
    QString uri;
    while (it.next()) {
        uri = it[0].uri().toString();
    }

    if (d->uri != uri) {
        d->uri = uri;
        d->res = Nepomuk2::Resource(uri);
        d->res.setWatchEnabled(true);
    }
}

QString PersonData::contactId() const
{
    Q_D(const PersonData);
    return d->id;
}

QString PersonData::uri() const
{
    Q_D(const PersonData);
    return d->uri;
}

void PersonData::setUri(const QString &uri)
{
    Q_D(PersonData);

    d->uri = uri;
    d->res = Nepomuk2::Resource(uri);
    d->res.setWatchEnabled(true);
}


QString PersonData::status() const
{
    Q_D(const PersonData);

    QStringList presenceList;

    if (d->res.type() == PIMO::Person()) {
        Q_FOREACH (const Nepomuk2::Resource &resource, d->res.property(PIMO::groundingOccurrence()).toResourceList()) {
            if (resource.hasProperty(NCO::hasIMAccount())) {
                QString imID = resource.property(NCO::hasIMAccount()).toResource().property(NCO::imID()).toString();
                presenceList << d->presenceModel->dataForContactId(imID, PersonsModel::StatusRole).toString();
            }
        }

        return findMostOnlinePresence(presenceList);
    } else {
        QString imID = d->res.property(NCO::hasIMAccount()).toResource().property(NCO::imID()).toString();
        return d->presenceModel->dataForContactId(imID, PersonsModel::StatusRole).toString();
    }
}

QUrl PersonData::avatar() const
{
    Q_D(const PersonData);

    Nepomuk2::Resource r;

    if (d->res.type() == PIMO::Person()) {
        Q_FOREACH (const Nepomuk2::Resource &resource, d->res.property(PIMO::groundingOccurrence()).toResourceList()) {
            if (resource.hasProperty(NCO::photo())) {
                r = resource;
                break;
            }
        }
    } else {
        r = d->res;
    }

    return r.property(NCO::photo()).toResource().property(NIE::url()).toUrl();
}

QString PersonData::name() const
{
    Q_D(const PersonData);

    QString label;
    Nepomuk2::Resource r;

    if (d->res.type() == PIMO::Person()) {
        //simply pick the first GO for now
        r = d->res.property(PIMO::groundingOccurrence()).toResource();
    } else {
        r = d->res;
    }

    label = r.property(NCO::hasEmailAddress()).toResource().property(NCO::fullname()).toString();

    if (!label.isEmpty()) {
        return label;
    }

    label = r.property(NCO::hasIMAccount()).toResource().property(NCO::imNickname()).toString();

    if (!label.isEmpty()) {
        return label;
    }

    return d->res.property(NCO::hasContactMedium()).toResource().genericLabel();
}

QStringList PersonData::emails() const
{
    Q_D(const PersonData);

    QStringList emails;

    if (d->res.type() == PIMO::Person()) {
        Q_FOREACH (const Nepomuk2::Resource &resource, d->res.property(PIMO::groundingOccurrence()).toResourceList()) {
            if (resource.hasProperty(NCO::hasEmailAddress())) {
                Q_FOREACH (const Nepomuk2::Resource &email, resource.property(NCO::hasEmailAddress()).toResourceList()) {
                    emails << email.property(NCO::emailAddress()).toString();
                }
            }
        }
    } else {
        if (d->res.hasProperty(NCO::hasEmailAddress())) {
            Q_FOREACH (const Nepomuk2::Resource &email, d->res.property(NCO::hasEmailAddress()).toResourceList()) {
                emails << email.property(NCO::emailAddress()).toString();
            }
        }
    }

    return emails;
}

QStringList PersonData::phones() const
{
    Q_D(const PersonData);

    QStringList phones;

    if (d->res.type() == PIMO::Person()) {
        Q_FOREACH (const Nepomuk2::Resource &resource, d->res.property(PIMO::groundingOccurrence()).toResourceList()) {
            if (resource.hasProperty(NCO::hasPhoneNumber())) {
                Q_FOREACH (const Nepomuk2::Resource &phone, resource.property(NCO::hasPhoneNumber()).toResourceList()) {
                    phones << phone.property(NCO::phoneNumber()).toString();
                }
            }
        }
    } else {
        if (d->res.hasProperty(NCO::hasPhoneNumber())) {
            Q_FOREACH (const Nepomuk2::Resource &phone, d->res.property(NCO::hasPhoneNumber()).toResourceList()) {
                phones << phone.property(NCO::phoneNumber()).toString();
            }
        }
    }

    return phones;
}

QStringList PersonData::imAccounts() const
{
    Q_D(const PersonData);

    QStringList ims;

    if (d->res.type() == PIMO::Person()) {
        Q_FOREACH (const Nepomuk2::Resource &resource, d->res.property(PIMO::groundingOccurrence()).toResourceList()) {
            if (resource.hasProperty(NCO::hasIMAccount())) {
                Q_FOREACH (const Nepomuk2::Resource &im, resource.property(NCO::hasIMAccount()).toResourceList()) {
                    ims << im.property(NCO::imAccountType()).toString();
                    ims << im.property(NCO::imNickname()).toString();
                    ims << im.property(NCO::imID()).toString();
                }
            }
        }
    } else {
        if (d->res.hasProperty(NCO::hasIMAccount())) {
            Q_FOREACH (const Nepomuk2::Resource &im, d->res.property(NCO::hasIMAccount()).toResourceList()) {
                ims << im.property(NCO::imAccountType()).toString();
                ims << im.property(NCO::imNickname()).toString();
                ims << im.property(NCO::imID()).toString();
            }
        }
    }

    return ims;
}


// QStringList PersonData::bareContacts() const
// {
//     return personIndex().data(PersonsModel::ContactIdRole).toStringList();
// }
//
// QModelIndex PersonData::personIndex() const
// {
//     Q_D(const PersonData);
//     Q_ASSERT(d->model->rowCount()>0);
//     QModelIndex idx = d->model->index(0,0);
//     Q_ASSERT(idx.isValid());
//     return idx;
// }
//

bool PersonData::isPerson() const
{
    Q_D(const PersonData);
    return d->res.type() == PIMO::Person();
}

QString PersonData::findMostOnlinePresence(const QStringList &presences) const
{
    if (presences.contains("available")) {
        return "available";
    }
    if (presences.contains("away")) {
        return "away";
    }
    if (presences.contains("busy") || presences.contains("dnd")) {
        return "busy";
    }
    if (presences.contains("xa")) {
        return "xa";
    }

    return "offline";
}
