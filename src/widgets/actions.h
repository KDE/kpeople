/*
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

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

#ifndef KPEOPLEWIDGETS_GLOBAL_H
#define KPEOPLEWIDGETS_GLOBAL_H

#include <QList>
#include <kpeople/kpeople_export.h>

class QString;
class QObject;
class QAction;
namespace KPeople
{
class PersonData;

/**
 * Each action returned in the list can be one of these
 * types, however the Type is not mandatory with the action
 *
 * The type should be set as QObject property "actionType"
 */
enum ActionType {
    TextChatAction,
    AudioCallAction,
    VideoCallAction,
    SendEmailAction,
    SendFileAction,
    OtherAction = 100
};

/**
 * Returns a list of actions relevant to the specified @p contactUri where
 * each QAction will have @p parent passed as its parent QObject
 */
KPEOPLE_EXPORT QList<QAction *> actionsForPerson(const QString &contactUri, QObject *parent);
}

#endif // KPEOPLEWIDGETS_GLOBAL_H
