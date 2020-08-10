/*
    SPDX-FileCopyrightText: 2013 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef PHONE_DETAILS_WIDGET_H
#define PHONE_DETAILS_WIDGET_H

#include "abstractpersondetailswidget.h"
#include <QVariant>

class PhoneDetailsWidget : public KPeople::AbstractPersonDetailsWidget
{
    Q_OBJECT
public:
    explicit PhoneDetailsWidget(QWidget *parent, const QVariantList &args);
    void setPerson(KPeople::PersonData *person);
};

#endif // PHONE_DETAILS_WIDGET_H
