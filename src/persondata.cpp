/*
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
#include "personitem_p.h"
#include "personpluginmanager.h"
#include "basepersonsdatasource.h"
#include "datasourcewatcher_p.h"

#include <Nepomuk2/Resource>
#include <Nepomuk2/Query/Query>
#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/ResourceWatcher>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Variant>

#include <Soprano/Model>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/QueryResultIterator>

#include <KDebug>

#include <QPointer>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

using namespace KPeople;

namespace KPeople {
class PersonDataPrivate {
public:
    PersonDataPrivate(PersonData *q)
    {
        dataSourceWatcher = new DataSourceWatcher(q);
        q->connect(dataSourceWatcher, SIGNAL(contactChanged(QUrl)), q, SIGNAL(dataChanged()));
    }

    QUrl uri;
    QPointer<Nepomuk2::ResourceWatcher> watcher;
    DataSourceWatcher *dataSourceWatcher;
    Nepomuk2::Resource personResource;
    QList<Nepomuk2::Resource> contactResources;
};
}

PersonDataPtr PersonData::createFromUri(const QUrl &uri)
{
    PersonDataPtr person(new PersonData);
    person->loadUri(uri);
    return person;
}

PersonDataPtr PersonData::createFromContactId(const QString &contactId)
{
    PersonDataPtr person(new PersonData);
    person->loadContact(contactId);
    return person;
}

PersonData::PersonData(QObject *parent)
    : QObject(parent),
      d_ptr(new PersonDataPrivate(this))
{
}

PersonData::~PersonData()
{
    delete d_ptr;
}

void PersonData::loadContact(const QString &id)
{
    QString query = QString::fromUtf8(
        "select DISTINCT ?uri ?person "
        "WHERE { "
            "OPTIONAL { ?person pimo:groundingOccurrence ?uri. }"
            "?uri a nco:PersonContact. "
            "?uri nco:hasContactMedium ?a . "
            "?a ?b \"%1\"^^xsd:string . "
        "}").arg(id);

    Soprano::Model *model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery(query, Soprano::Query::QueryLanguageSparql);
    QString uri;
    while (it.next()) {
        const QString person = it[1].uri().toString();
        if (person.isEmpty()) {
            uri = it[0].uri().toString();
        } else {
            uri = person;
        }
        break;
    }
    loadUri(uri);
}

QUrl PersonData::uri() const
{
    Q_D(const PersonData);
    return d->uri;
}

void PersonData::loadUri(const QUrl &uri)
{
    Q_D(PersonData);

    d->uri = uri;
    d->personResource = Nepomuk2::Resource();

    Nepomuk2::Resource r(uri);

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
    connect(d->watcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SIGNAL(dataChanged()));
    d->watcher->start();

    //watch for IM changes
    Q_FOREACH (const Nepomuk2::Resource &resource, d->contactResources) {
        Q_FOREACH (const Nepomuk2::Resource &im, resource.property(NCO::hasIMAccount()).toResourceList()) {
            d->dataSourceWatcher->watchContact(im.property(NCO::imID()).toString(), resource.uri());
        }
    }

    emit dataChanged();
}

bool PersonData::isValid() const
{
    Q_D(const PersonData);
    return !d->contactResources.isEmpty();
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

    if (d->contactResources.isEmpty()) {
        return QString();
    }

    QString label;
    //simply pick the first for now
    Nepomuk2::Resource r = d->contactResources.first();

    if (r.hasProperty(NCO::nickname())) {
        label = r.property(NCO::nickname()).toString();
    } else if (r.hasProperty(NAO::prefLabel())) {
        label = r.property(NAO::prefLabel()).toString();
    } else if (r.hasProperty(NCO::fullname())) {
        label = r.property(NCO::fullname()).toString();
    } else if (r.hasProperty(NCO::hasIMAccount())) {
        label = r.property(NCO::hasIMAccount()).toResource().property(NCO::imNickname()).toString();
    } else if (r.hasProperty(NCO::hasEmailAddress())) {
        label = r.property(NCO::hasEmailAddress()).toResource().property(NCO::emailAddress()).toString();
    } else if (r.hasProperty(NCO::hasPhoneNumber())) {
        label = r.property(NCO::hasPhoneNumber()).toResource().property(NCO::phoneNumber()).toString();
    }

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
