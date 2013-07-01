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


#ifndef DECLARATIVEPERSONSMODEL_H
#define DECLARATIVEPERSONSMODEL_H

#include "personsmodel.h"

#include <QDeclarativeParserStatus>

class DeclarativePersonsModel: public KPeople::PersonsModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(Features mandatoryFeatures READ mandatoryFeatures WRITE setMandatoryFeatures)
    Q_PROPERTY(Features optionalFeatures READ optionalFeatures WRITE setOptionalFeatures)

public:
        enum Feature {
        FeatureNone       = 0x0000,
        FeatureIM         = 0x0001,
        FeatureGroups     = 0x0002,
        FeatureAvatars    = 0x0004,
        FeatureEmails     = 0x0008,
        FeatureFullName   = 0x0010,
        FeatureAll = FeatureIM |
                     FeatureGroups |
                     FeatureAvatars |
                     FeatureEmails |
                     FeatureFullName
    };

    Q_DECLARE_FLAGS(Features, Feature)
    Q_FLAGS(Features)

    DeclarativePersonsModel(QObject *parent = 0);

    void classBegin();
    void componentComplete();

    Features mandatoryFeatures() const;
    void setMandatoryFeatures(Features);

    Features optionalFeatures() const;
    void setOptionalFeatures(Features);

private:
    Features m_optionalFeatures;
    Features m_mandatoryFeatures;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DeclarativePersonsModel::Features)


#endif // DECLARATIVEPERSONSMODEL_H
