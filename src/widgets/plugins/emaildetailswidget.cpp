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


#include "emaildetailswidget.h"
#include "persondata.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

#include <KLocalizedString>

using namespace KPeople;

EmailDetailsWidget::EmailDetailsWidget(QWidget *parent):
    AbstractPersonDetailsWidget(parent)
{
    setTitle(i18n("Email"));
    setIcon(KIcon("mail-message"));
    QVBoxLayout *newLayout = new QVBoxLayout(this);
    newLayout->setContentsMargins(0,0,0,0);
    setLayout(newLayout);
}

void EmailDetailsWidget::setPerson(PersonData *person)
{
    if(person->emails().size() == 0) {
        setActive(false);
    } else {
        setActive(true);
    }

    QLayoutItem *child;
    while ((child = layout()->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }

    Q_FOREACH (const QString &email, person->emails()) {
        QLabel *emailLabel = new QLabel(email, this);
        layout()->addWidget(emailLabel);
    }
}
