/*
    Copyright (C) 2013  David Edmundson <D.Edmundson@lboro.ac.uk>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef DECLARATIVEPERSONDATA_H
#define DECLARATIVEPERSONDATA_H

#include "persondata.h"

#include <QDeclarativeParserStatus>

class DeclarativePersonData : public KPeople::PersonData, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QString uri WRITE setUri);
    Q_PROPERTY(QString contactId WRITE setContactId);

public:
    DeclarativePersonData(QObject *parent=0);

    void setUri(const QString &uri);
    void setContactId(const QString &uri);


    void classBegin();
    void componentComplete();

private:
    QString m_uri;
    QString m_contactId;
};

#endif // DECLARATIVEPERSONDATA_H
