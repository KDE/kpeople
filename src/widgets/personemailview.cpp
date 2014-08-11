/*
    Copyright 2014  Nilesh Suthar <nileshsuthar@live.in>

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

#include "personemailview.h"

#include "abstractfieldwidgetfactory.h"
#include "plugins/emaildetailswidget.h"
#include "plugins/emails.h"
#include "global.h"

#include <QVBoxLayout>
#include <QFormLayout>

namespace KPeople {

class PersonEmailsViewPrivate {
public:
    PersonData  *person;
    QWidget *mainWidget;
    Emails *emails;
};

using namespace KPeople;

PersonEmailsView::PersonEmailsView(QWidget *parent)
    : QWidget(parent),
      d_ptr(new PersonEmailsViewPrivate())
{
    Q_D(PersonEmailsView);
    setLayout(new QVBoxLayout(this));
    d->mainWidget = new QWidget(this);
    d->person = 0;
    d->emails = new Emails();

}

PersonEmailsView::~PersonEmailsView()
{
    delete d_ptr;
}

void PersonEmailsView::setPerson(PersonData *person)
{
    Q_D(PersonEmailsView);
    if (d->person) {
        disconnect(d->person, SIGNAL(dataChanged()), this, SLOT(reload()));
    }

    d->person = person;
    connect(d->person, SIGNAL(dataChanged()), this, SLOT(reload()));
    reload();
}

void PersonEmailsView::reload()
{
    Q_D(PersonEmailsView);

    d->mainWidget->deleteLater();
    d->mainWidget = d->emails->createDetailsWidget(d->person->person(), d->person->contacts(), this);
    layout()->addWidget(d->mainWidget);
}
}
