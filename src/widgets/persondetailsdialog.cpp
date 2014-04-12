/*
 * Convience dialog for displaying PersonDetailsView widgets
 * Copyright 2013  David Edmundson <davidedmundson@kde.org>
 *
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


#include "persondetailsdialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>

#include "persondetailsview.h"

using namespace KPeople;

namespace KPeople {
class PersonDetailsDialogPrivate {
public:
    PersonDetailsView *view;
};
}

PersonDetailsDialog::PersonDetailsDialog(QWidget *parent, Qt::WindowFlags f):
    QDialog(parent, f),
    d_ptr(new PersonDetailsDialogPrivate)
{
    Q_D(PersonDetailsDialog);
    QVBoxLayout *layout = new QVBoxLayout(this);

    d->view = new PersonDetailsView(this);
    layout->addWidget(d->view);

    QDialogButtonBox *buttons = new QDialogButtonBox(this);
    buttons->setStandardButtons(QDialogButtonBox::Ok);
    layout->addWidget(buttons);

    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));

    setLayout(layout);
}

PersonDetailsDialog::~PersonDetailsDialog()
{
    delete d_ptr;
}

void PersonDetailsDialog::setPerson(PersonData *person)
{
    Q_D(PersonDetailsDialog);
    d->view->setPerson(person);

    setWindowTitle(person->person().formattedName());
}


#include "persondetailsdialog.moc"
