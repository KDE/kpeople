/*
 * Copyright 2014  Nilesh Suthar <nileshsuthar@live.in>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "personnoteview.h"

#include "abstractfieldwidgetfactory.h"
#include "plugins/note.h"
#include "global.h"

#include <QFormLayout>
#include <QVBoxLayout>

namespace KPeople {

class PersonNoteViewPrivate {
public:
    PersonData  *m_person;
    QWidget *m_mainWidget;
    AbstractFieldWidgetFactory *m_note;
};

using namespace KPeople;

PersonNoteView::PersonNoteView(QWidget *parent)
    : QWidget(parent),
      d_ptr(new PersonNoteViewPrivate())
{
    Q_D(PersonNoteView);
    setLayout(new QVBoxLayout(this));
    d->m_mainWidget = new QWidget(this);
    d->m_person = 0;

    d->m_note = new Note();

}

PersonNoteView::~PersonNoteView()
{
    delete d_ptr;
}

void PersonNoteView::setPerson(PersonData *person)
{
    Q_D(PersonNoteView);
    if (d->m_person) {
        disconnect(d->m_person, SIGNAL(dataChanged()), this, SLOT(reload()));
    }

    d->m_person = person;
    connect(d->m_person, SIGNAL(dataChanged()), this, SLOT(reload()));
    reload();
}

void PersonNoteView::reload()
{
    Q_D(PersonNoteView);

    //replace the entire main widget
    int layoutIndex = layout()->indexOf(d->m_mainWidget);
    layout()->takeAt(layoutIndex);
    d->m_mainWidget->deleteLater();
    d->m_mainWidget = new QWidget(this);
    dynamic_cast<QVBoxLayout *>(layout())->insertWidget(layoutIndex, d->m_mainWidget);

    QFormLayout *layout = new QFormLayout(d->m_mainWidget);
    layout->setSpacing(4);

    QWidget *widget = d->m_note->createDetailsWidget(d->m_person->person(), d->m_person->contacts(), this);

    layout->addRow(widget);

}

}

#include "personnoteview.moc"
