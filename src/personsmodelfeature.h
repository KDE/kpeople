/*
    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>
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


#ifndef PERSONS_MODEL_FEATURE_H
#define PERSONS_MODEL_FEATURE_H

#include <QHash>
#include <QString>
#include <QUrl>

#include "personsmodel.h"

class PersonsModelFeaturePrivate;

class PersonsModelFeature
{
public:
    /**
    * @class PersonsModelFeature create mapping between nepomuk ontologies
    * New instances can be created to fetch other Nepomuk data
    *
    * As a general rule one the static methods can be used to create features for common model functions
    * It can be used to fetch additional Nepomuk contact data in the model
    *
    * For example to map the nco:emailAddress to the EmailRole the code will be as follows
    *
    *   PersonsModelFeature emailsFeature;
    *   emailsFeature.setQueryPart(QString::fromUtf8(
    *       "?uri                    nco:hasEmailAddress    ?nco_hasEmailAddress. "
    *       "?nco_hasEmailAddress    nco:emailAddress       ?nco_emailAddress. "));
    *   QHash<QString, PersonsModel::Role> eb;
    *   eb.insert("nco_emailAddress", EmailRole);
    *   emailsFeature.setBindingsMap(eb);
    *   emailsFeature.setOptional(false);
    *   emailsFeature.setWatcherProperty(Nepomuk2::Vocabulary::NCO::hasEmailAddress());
    */
    PersonsModelFeature();
    PersonsModelFeature(const PersonsModelFeature &other);
    virtual ~PersonsModelFeature();

    PersonsModelFeature &operator=(const PersonsModelFeature &other);

    /** Set the relation between the binding in the SopranoQueryResult and the model roles
    *   Model data is automatically set for any item in the map
    */
    void setBindingsMap(QHash<QString, int> bindingsMap);
    QHash<QString, int> bindingsMap() const;

    /** Set the raw nepomuk query needed for this piece of information
    *  query should set one or more parameters used in the binding map
    *
    *  ?uri contains the URI for the current contact
    *  The selected variables are automatically inferred from this part of the query
    *
    *  The query can return multiple values. This will be handled by the model.
    */
    void setQueryPart(const QString &part);

    //TODO this returns something different to d->queryPart which is confusing
    //suggest making this return d->queryPart, and making the method that does exist have a different name
    QString queryPart() const;

    /**
     * Set whether this queryPart needs to find results in order to include the URI
     *
     * For the email example, setting this to true will only include contacts with an email address
     * With @optional set to false, emails will be set if they exist, otherwise will be empty
     *     *
     * From a nepomuk perspective if this is true the queryPart wrapped in "OPTIONAL {}"
     */
    void setOptional(bool optional);
    bool isOptional() const;

    /** Sets the nepomuk property for the contact that will be watched for changes
     */
    void setWatcherProperty(const QUrl &property);
    QUrl watcherProperty() const;

    static PersonsModelFeature imModelFeature(bool optional=true);
    static PersonsModelFeature groupsModelFeature(bool optional=true);
    static PersonsModelFeature avatarModelFeature(bool optional=true);
    static PersonsModelFeature emailModelFeature(bool optional=true);
    static PersonsModelFeature fullNameModelFeature(bool optional=true);

private:
    QSharedDataPointer<PersonsModelFeaturePrivate> d;
};

#endif // PERSONS_MODEL_FEATURE_H
