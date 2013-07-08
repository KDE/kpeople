/*
    Copyright (C) 2013  David Edmundson <D.Edmundson@lboro.ac.uk>

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
