/*
    <one line to give the program's name and a brief idea of what it does.>
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

#ifndef ABSTRACT_PERSON_PLUGIN_H
#define ABSTRACT_PERSON_PLUGIN_H

class PersonData;

#include <QObject>
#include <QAction>

class AbstractPersonPlugin : public QObject
{

public:
    AbstractPersonPlugin(QObject *parent);
    /** A list of actions that can be executed for a contact
     * Disabled actions should not be added
     *
     * @personData information on a single contact
     * @parent parent object for newly created actions
     */
    virtual QList<QAction*> actionsForPerson(PersonData * const personData, QObject *parent);

    /** Return a custom widget to show detailed information about a contact
     */
//     virtual QList<AbstractPersonDetailsWidget*> detailWidgets(PersonData * const personData, QWidget *parent);
};

#endif