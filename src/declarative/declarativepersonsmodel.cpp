/*
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


#include "declarativepersonsmodel.h"

using namespace KPeople;

#include "personsmodelfeature.h"
#include <KDebug>

DeclarativePersonsModel::DeclarativePersonsModel(QObject *parent)
    : PersonsModel(parent),
      m_optionalFeatures(0),
      m_mandatoryFeatures(0)
{

}

void DeclarativePersonsModel::classBegin()
{
    //needed for QDeclarativeParserStatus.. we don't use it though
}

DeclarativePersonsModel::Features DeclarativePersonsModel::mandatoryFeatures() const
{
    return m_mandatoryFeatures;
}

void DeclarativePersonsModel::setMandatoryFeatures(DeclarativePersonsModel::Features features)
{
    m_mandatoryFeatures = features;
}

DeclarativePersonsModel::Features DeclarativePersonsModel::optionalFeatures() const
{
    return m_optionalFeatures;
}

void DeclarativePersonsModel::setOptionalFeatures(DeclarativePersonsModel::Features features)
{
    m_optionalFeatures = features;
}

void DeclarativePersonsModel::componentComplete()
{
    QList<PersonsModelFeature> features;

    //do nothing if empty flags are passed
    if (m_mandatoryFeatures == 0 && m_optionalFeatures == 0) {
        kWarning() << "null query flags passed!";
        return;
    }

    //FIXME there is a bug in which a feature is in both mandatory and optional it is included twice.

    if (m_mandatoryFeatures & DeclarativePersonsModel::FeatureIM) {
        kDebug() << "Adding mandatory IM";
        features << PersonsModelFeature::imModelFeature(PersonsModelFeature::Mandatory);
    }

    if (m_mandatoryFeatures & DeclarativePersonsModel::FeatureAvatars) {
        kDebug() << "Adding mandatory Avatars";
        features << PersonsModelFeature::avatarModelFeature(PersonsModelFeature::Mandatory);
    }

    if (m_mandatoryFeatures & DeclarativePersonsModel::FeatureGroups) {
        kDebug() << "Adding mandatory Groups";
        features << PersonsModelFeature::groupsModelFeature(PersonsModelFeature::Mandatory);
    }

    if (m_mandatoryFeatures & DeclarativePersonsModel::FeatureEmails) {
        kDebug() << "Adding mandatory Emails";
        features << PersonsModelFeature::emailModelFeature(PersonsModelFeature::Mandatory);
    }

    if (m_mandatoryFeatures & DeclarativePersonsModel::FeatureFullName) {
        kDebug() << "Adding mandatory FullName";
        features << PersonsModelFeature::fullNameModelFeature(PersonsModelFeature::Mandatory);
    }

    if (m_optionalFeatures & DeclarativePersonsModel::FeatureIM) {
        kDebug() << "Adding optional IM";
        features << PersonsModelFeature::imModelFeature(PersonsModelFeature::Optional);
    }

    if (m_optionalFeatures & DeclarativePersonsModel::FeatureAvatars) {
        kDebug() << "Adding optional Avatars";
        features << PersonsModelFeature::avatarModelFeature(PersonsModelFeature::Optional);
    }

    if (m_optionalFeatures & DeclarativePersonsModel::FeatureGroups) {
        kDebug() << "Adding optional Groups";
        features << PersonsModelFeature::groupsModelFeature(PersonsModelFeature::Optional);
    }

    if (m_optionalFeatures & DeclarativePersonsModel::FeatureEmails) {
        kDebug() << "Adding optional Emails";
        features << PersonsModelFeature::emailModelFeature(PersonsModelFeature::Optional);
    }

    if (m_optionalFeatures & DeclarativePersonsModel::FeatureFullName) {
        kDebug() << "Adding optional FullName";
        features << PersonsModelFeature::fullNameModelFeature(PersonsModelFeature::Optional);
    }

    startQuery(features);
}
