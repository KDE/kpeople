/*
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


#include "personsmodelfeature.h"

#include <Nepomuk2/Vocabulary/NCO>

class PersonsModelFeaturePrivate : public QSharedData
{
public:
    QHash<QString, int> bindingsMap;
    QString queryPart;
    bool isOptional;
    QUrl watcherProperty;
};

PersonsModelFeature::PersonsModelFeature()
: d (new PersonsModelFeaturePrivate)
{
}

PersonsModelFeature::PersonsModelFeature(const PersonsModelFeature &other)
: d (other.d)
{
}

PersonsModelFeature::~PersonsModelFeature()
{
    //note do not need to delete d as it is a QSharedData
}

PersonsModelFeature& PersonsModelFeature::operator=(const PersonsModelFeature& other)
{
    if (this == &other) {
        return *this; //Protect against self-assignment
    }
    d = other.d;
    return *this;
}

QHash<QString, int> PersonsModelFeature::bindingsMap() const
{
    return d->bindingsMap;
}

void PersonsModelFeature::setBindingsMap(QHash<QString, int> bindingsMap)
{
    d->bindingsMap = bindingsMap;
}

void PersonsModelFeature::setQueryPart(const QString &part)
{
    d->queryPart = part;
}

QString PersonsModelFeature::queryPart() const
{
    if (d->isOptional) {
        return QString("OPTIONAL { %1 } ").arg(d->queryPart);
    } else {
        return d->queryPart;
    }
}

bool PersonsModelFeature::isOptional() const
{
    return d->isOptional;
}

void PersonsModelFeature::setOptional(bool optional)
{
    d->isOptional = optional;
}

QUrl PersonsModelFeature::watcherProperty() const
{
    return d->watcherProperty;
}

void PersonsModelFeature::setWatcherProperty(const QUrl &property)
{
    d->watcherProperty = property;
}


PersonsModelFeature PersonsModelFeature::avatarModelFeature(bool optional)
{
    PersonsModelFeature avatarsFeature;
    avatarsFeature.setQueryPart(QString::fromUtf8(
        "?uri      nco:photo    ?phRes. "
        "?phRes    nie:url      ?nie_url. "));
    QHash<QString, int> pb;
    pb.insert("nie_url", PersonsModel::PhotosRole);
    avatarsFeature.setBindingsMap(pb);
    avatarsFeature.setOptional(optional);
    avatarsFeature.setWatcherProperty(Nepomuk2::Vocabulary::NCO::photo());
    return avatarsFeature;
}

PersonsModelFeature PersonsModelFeature::emailModelFeature(bool optional)
{
    PersonsModelFeature emailsFeature;
    emailsFeature.setQueryPart(QString::fromUtf8(
        "?uri                    nco:hasEmailAddress    ?nco_hasEmailAddress. "
        "?nco_hasEmailAddress    nco:emailAddress       ?nco_emailAddress. "));
    QHash<QString, int> eb;
    eb.insert("nco_emailAddress", PersonsModel::EmailsRole);
    emailsFeature.setBindingsMap(eb);
    emailsFeature.setOptional(optional);
    emailsFeature.setWatcherProperty(Nepomuk2::Vocabulary::NCO::hasEmailAddress());
    return emailsFeature;
}

PersonsModelFeature PersonsModelFeature::fullNameModelFeature(bool optional)
{
    PersonsModelFeature fullNameFeature;
    fullNameFeature.setQueryPart(QString::fromUtf8(
        "?uri                    nco:fullname    ?nco_fullname. "));
    QHash<QString, int> fnb;
    fnb.insert("nco_fullname", PersonsModel::FullNamesRole);
    fullNameFeature.setBindingsMap(fnb);
    fullNameFeature.setOptional(optional);
    return fullNameFeature;
}

PersonsModelFeature PersonsModelFeature::groupsModelFeature(bool optional)
{
    PersonsModelFeature groupsFeature;
    groupsFeature.setQueryPart(QString::fromUtf8(
        "?uri                   nco:belongsToGroup      ?nco_belongsToGroup . "
        "?nco_belongsToGroup    nco:contactGroupName    ?nco_contactGroupName . "));
    QHash<QString, int> gb;
    gb.insert("nco_contactGroupName", PersonsModel::GroupsRole);
    groupsFeature.setBindingsMap(gb);
    groupsFeature.setOptional(optional);
    groupsFeature.setWatcherProperty(Nepomuk2::Vocabulary::NCO::belongsToGroup());
    return groupsFeature;
}

PersonsModelFeature PersonsModelFeature::imModelFeature(bool optional)
{
    PersonsModelFeature imFeature;
    imFeature.setQueryPart(QString::fromUtf8(
        "?uri                 nco:hasIMAccount     ?nco_hasIMAccount. "
        "?nco_hasIMAccount    nco:imNickname       ?nco_imNickname. "
        "?nco_hasIMAccount    nco:imID             ?nco_imID. "));
    QHash<QString, int> b;
    b.insert("nco_imNickname", PersonsModel::NicknamesRole);
    b.insert("nco_imID", PersonsModel::IMsRole);
    imFeature.setBindingsMap(b);
    imFeature.setOptional(optional);
    imFeature.setWatcherProperty(Nepomuk2::Vocabulary::NCO::hasIMAccount());
    return imFeature;
}
