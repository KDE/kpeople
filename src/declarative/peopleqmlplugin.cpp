/*
    Persons Model
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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

#include "peopleqmlplugin.h"

#include <personsmodel.h>
#include <personactionsmodel_p.h>
#include <persondata.h>
#include <personssortfilterproxymodel.h>
#include <personpluginmanager.h>
#include <actions.h>

#include "declarativepersondata.h"
#include <qqml.h>

class ActionTypeWrapper : public QObject {
    Q_OBJECT

public:
    enum ActionType {
        TextChatAction = KPeople::TextChatAction,
        AudioCallAction = KPeople::AudioCallAction,
        VideoCallAction = KPeople::VideoCallAction,
        SendEmailAction = KPeople::SendEmailAction,
        SendFileAction = KPeople::SendFileAction,
        OtherAction = KPeople::OtherAction
    };
    Q_ENUM(ActionType)
};

class DeclarativePersonPluginManager : public QObject
{
    Q_OBJECT
public:
    Q_SCRIPTABLE bool addContact(const QVariantMap &properties) {
        return KPeople::PersonPluginManager::addContact(properties);
    }
    Q_SCRIPTABLE bool deleteContact(const QString &uri) {
        return KPeople::PersonPluginManager::deleteContact(uri);
    }
};

void PeopleQMLPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<KPeople::PersonsModel>(uri, 1, 0, "PersonsModel");
    qmlRegisterType<KPeople::PersonsSortFilterProxyModel>(uri, 1, 0, "PersonsSortFilterProxyModel");
    qmlRegisterType<KPeople::PersonActionsModel>(uri, 1, 0, "PersonActions");
    qmlRegisterType<DeclarativePersonData>(uri, 1, 0, "PersonData");
    qmlRegisterType<KPeople::PersonData>();
    qmlRegisterUncreatableType<ActionTypeWrapper>(uri, 1, 0, "ActionType", QStringLiteral("You cannot create ActionType"));
    qmlRegisterSingletonType<DeclarativePersonPluginManager>(uri, 1, 0, "PersonPluginManager", [] (QQmlEngine*, QJSEngine*) -> QObject* { return new DeclarativePersonPluginManager; });
}

#include "peopleqmlplugin.moc"
