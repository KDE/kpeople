/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2014  <copyright holder> <email>
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

#include "note.h"

#include <QVBoxLayout>
#include <QLabel>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <QDebug>
#include <KPluginFactory>
#include <KLocalizedString>

#include <KABC/Addressee>
#include <KPluginFactory>
#include <KPluginLoader>

#include <QDebug>

Note::Note(QObject* parent): AbstractFieldWidgetFactory(parent)
{
}

QWidget* Note::createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList &contacts, QWidget* parent) const
{
    Q_UNUSED(contacts);
    QWidget *widget = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0,0,0,0);
    QTextEdit *noteEditor = new QTextEdit(parent);
    QPushButton *save = new QPushButton("Save");
    noteEditor->setText(person.note());

    if(person.note() != "")
        noteEditor->setDisabled(true);

    connect(save,SIGNAL(clicked(bool)),this,SLOT(saveNote(bool)));
    connect(noteEditor,SIGNAL(textChanged()),this,SLOT(textChanged()));
    layout->addWidget(noteEditor);
    layout->addWidget(save);
    widget->setLayout(layout);
    return widget;
}

void Note::saveNote(bool)
{
 //TODO On save get qtextedit refrence and save the text
}

void Note::textChanged()
{
  //TODO enable save button on text changed
}


QString Note::label() const
{
    return i18n("Mails");
}

int Note::sortWeight() const
{
    return 0;
}

#include "note.moc"
