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

#include <QQmlParserStatus>

class DeclarativePersonData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id WRITE setId NOTIFY personChanged);

public:
    DeclarativePersonData(QObject *parent=0);

    void setId(const QString &uri);

Q_SIGNALS:
    void personChanged();

private:
    QString m_id;
    KPeople::PersonData* m_person;
};

#endif // DECLARATIVEPERSONDATA_H
