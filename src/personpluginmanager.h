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


#ifndef PERSON_PLUGIN_MANAGER_H
#define PERSON_PLUGIN_MANAGER_H

#include <QObject>

#include "kpeople_export.h"

class BasePersonsDataSource;
class QAction;
class PersonData;

class KPEOPLE_EXPORT PersonPluginManager
{
public:
    static QList<QAction*> actionsForPerson(PersonData *person, QObject *parent);

    static BasePersonsDataSource* presencePlugin();
};

#endif // PERSON_PLUGIN_MANAGER_H
