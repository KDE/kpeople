/*
    SPDX-FileCopyrightText: 2013 David Edmundson <D.Edmundson@lboro.ac.uk>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef DECLARATIVEPERSONDATA_H
#define DECLARATIVEPERSONDATA_H

#include "persondata.h"

class DeclarativePersonData : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString personUri READ personUri WRITE setPersonUri NOTIFY personChanged)
    Q_PROPERTY(KPeople::PersonData *person READ person NOTIFY personChanged)

public:
    explicit DeclarativePersonData(QObject *parent = nullptr);

    void setPersonUri(const QString &id);
    QString personUri() const;

    KPeople::PersonData *person() const;

Q_SIGNALS:
    void personChanged();

private:
    QString m_id;
    KPeople::PersonData *m_person = nullptr;
};

#endif // DECLARATIVEPERSONDATA_H
