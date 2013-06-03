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
#include "personsmodel.h"
#include "personitem.h"
#include "personpluginmanager.h"
#include "basepersonsdatasource.h"

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
#include <QPointer>

using namespace Nepomuk2::Vocabulary;

class PersonDataPrivate {
public:
    QString uri;
    QString id;
    QPointer<Nepomuk2::ResourceWatcher> watcher;
    Nepomuk2::Resource personResource;
    QList<Nepomuk2::Resource> contactResources;
};

PersonData::PersonData(QObject *parent)
    : QObject(parent),
    d_ptr(new PersonDataPrivate)
{
}

PersonData::PersonData(const QString &uri, QObject *parent)
    : QObject(parent),
      d_ptr(new PersonDataPrivate)
{
    setUri(uri);
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
    Soprano::QueryResultIterator it = model->executeQuery(query, Soprano::Query::QueryLanguageSparql);
    QString uri;
    while (it.next()) {
        uri = it[0].uri().toString();
    }

    if (d->uri != uri) {
        setUri(uri);
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
    d->contactResources.clear();
    d->personResource = Nepomuk2::Resource();

    Nepomuk2::Resource r(uri);

    if (!d->watcher.isNull()) {
        delete d->watcher;
    }

    d->watcher = new Nepomuk2::ResourceWatcher(this);

    if (r.type() == PIMO::Person()) {
        d->personResource = r;
        d->contactResources = r.property(PIMO::groundingOccurrence()).toResourceList();

        Q_FOREACH (Nepomuk2::Resource res, d->contactResources) { //cannot use const here as we're modifying the resource
            d->watcher->addResource(res);
            res.setWatchEnabled(true);
        }
    } else {
        d->contactResources = QList<Nepomuk2::Resource>() << r;
    }

    r.setWatchEnabled(true);
    d->watcher->addResource(r);

    connect(d->watcher.data(), SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SIGNAL(dataChanged()));

    emit uriChanged();
    emit dataChanged();
}


QString PersonData::status() const
{
    Q_D(const PersonData);

    QStringList presenceList;

    Q_FOREACH (const Nepomuk2::Resource &resource, d->contactResources) {
        if (resource.hasProperty(NCO::hasIMAccount())) {
            QString imID = resource.property(NCO::hasIMAccount()).toResource().property(NCO::imID()).toString();
            presenceList << PersonPluginManager::presencePlugin()->dataForContact(imID, PersonsModel::PresenceTypeRole).toString();
        }
    }

    return findMostOnlinePresence(presenceList);
}

QUrl PersonData::avatar() const
{
    Q_D(const PersonData);

    Q_FOREACH (const Nepomuk2::Resource &resource, d->contactResources) {
        if (resource.hasProperty(NCO::photo())) {
            return resource.property(NCO::photo()).toResource().property(NIE::url()).toUrl();
        }
    }

    return QUrl();
}

QString PersonData::name() const
{
    Q_D(const PersonData);

    QString label;
    //simply pick the first for now
    Nepomuk2::Resource r = d->contactResources.first();

    label = r.property(NCO::fullname()).toString();

    if (!label.isEmpty()) {
        return label;
    }

    label = r.property(NCO::hasIMAccount()).toResource().property(NCO::imNickname()).toString();

    if (!label.isEmpty()) {
        return label;
    }

    return r.property(NCO::hasContactMedium()).toResource().genericLabel();
}

QStringList PersonData::emails() const
{
    Q_D(const PersonData);

    QStringList emails;

    Q_FOREACH (const Nepomuk2::Resource &resource, d->contactResources) {
        Q_FOREACH (const Nepomuk2::Resource &email, resource.property(NCO::hasEmailAddress()).toResourceList()) {
            emails << email.property(NCO::emailAddress()).toString();
        }
    }

    return emails;
}

QStringList PersonData::phones() const
{
    Q_D(const PersonData);

    QStringList phones;

    Q_FOREACH (const Nepomuk2::Resource &resource, d->contactResources) {
        Q_FOREACH (const Nepomuk2::Resource &phone, resource.property(NCO::hasPhoneNumber()).toResourceList()) {
            phones << phone.property(NCO::phoneNumber()).toString();
        }
    }

    return phones;
}

QStringList PersonData::imAccounts() const
{
    Q_D(const PersonData);

    QStringList ims;

    Q_FOREACH (const Nepomuk2::Resource &resource, d->contactResources) {
        Q_FOREACH (const Nepomuk2::Resource &im, resource.property(NCO::hasIMAccount()).toResourceList()) {
            ims << im.property(NCO::imAccountType()).toString();
            ims << im.property(NCO::imNickname()).toString();
            ims << im.property(NCO::imID()).toString();
        }
    }

    return ims;
}

KDateTime PersonData::birthday() const
{
    Q_D(const PersonData);

    //we'll go through all the dates we have and from every date we
    //get msecs from epoch and then we'll check whichever is greater
    //If the date has only month and a year, it will be counted
    //to 1st day of that month. If the real date is actually 15th day,
    //it means the more complete date will have more msecs from the epoch
    KDateTime bd;

    Q_FOREACH (const Nepomuk2::Resource &resource, d->contactResources) {
        KDateTime bdTemp(resource.property(NCO::birthDate()).toDateTime());
        if (bdTemp.isValid() && bdTemp.dateTime().toMSecsSinceEpoch() > bd.dateTime().toMSecsSinceEpoch()) {
            bd = bdTemp;
        }
    }

    return bd;
}

QStringList PersonData::groups() const
{
    Q_D(const PersonData);

    QStringList groups;

    Q_FOREACH (const Nepomuk2::Resource &resource, d->contactResources) {
        Nepomuk2::Variant groupProperties = resource.property(NCO::belongsToGroup());
        if (!groupProperties.isValid()) {
            continue;
        }

        Q_FOREACH (const Nepomuk2::Resource &groupResource, groupProperties.toResourceList()) {
            groups << groupResource.property(NCO::contactGroupName()).toString();
        }
    }

    return groups;
}

QList< Nepomuk2::Resource > PersonData::contactResources() const
{
    Q_D(const PersonData);
    return d->contactResources;
}


bool PersonData::isPerson() const
{
    Q_D(const PersonData);
    return d->personResource.isValid();
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


