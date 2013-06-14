/*
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

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


#include "phonedetailswidget.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

#include <KLocalizedString>

#include "persondata.h"

PhoneDetailsWidget::PhoneDetailsWidget(QWidget *parent):
    AbstractPersonDetailsWidget(parent)
{
    setTitle(i18n("Phone"));
    setIcon(KIcon("phone"));
}

void PhoneDetailsWidget::setPerson(PersonData* person)
{
    if(person->phones().size() == 0) {
        setActive(false);
    } else {
        setActive(true);
    }

    if (layout()) {
        layout()->deleteLater();
    }
    QVBoxLayout *newLayout = new QVBoxLayout(this);
    newLayout->setContentsMargins(0,0,0,0);
    Q_FOREACH (const QString &phone, person->phones()) {
        QLabel *phoneLabel = new QLabel(phone, this);
        newLayout->addWidget(phoneLabel);
    }
    setLayout(newLayout);
}
