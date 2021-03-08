/*
    Persons Model
    SPDX-FileCopyrightText: 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "peopleqmlplugin.h"

#include <actions.h>
#include <personactionsmodel_p.h>
#include <persondata.h>
#include <personpluginmanager.h>
#include <personsmodel.h>
#include <personssortfilterproxymodel.h>

#include "declarativepersondata.h"
#include <qqml.h>

class ActionTypeWrapper : public QObject
{
    Q_OBJECT

public:
    enum ActionType {
        TextChatAction = KPeople::TextChatAction,
        AudioCallAction = KPeople::AudioCallAction,
        VideoCallAction = KPeople::VideoCallAction,
        SendEmailAction = KPeople::SendEmailAction,
        SendFileAction = KPeople::SendFileAction,
        OtherAction = KPeople::OtherAction,
    };
    Q_ENUM(ActionType)
};

class DeclarativePersonPluginManager : public QObject
{
    Q_OBJECT
public:
    Q_SCRIPTABLE bool addContact(const QVariantMap &properties)
    {
        return KPeople::PersonPluginManager::addContact(properties);
    }
    Q_SCRIPTABLE bool deleteContact(const QString &uri)
    {
        return KPeople::PersonPluginManager::deleteContact(uri);
    }
};

void PeopleQMLPlugin::registerTypes(const char *uri)
{
    qmlRegisterType<KPeople::PersonsModel>(uri, 1, 0, "PersonsModel");
    qmlRegisterType<KPeople::PersonsSortFilterProxyModel>(uri, 1, 0, "PersonsSortFilterProxyModel");
    qmlRegisterType<KPeople::PersonActionsModel>(uri, 1, 0, "PersonActions");
    qmlRegisterType<DeclarativePersonData>(uri, 1, 0, "PersonData");
    qmlRegisterAnonymousType<KPeople::PersonData>(uri, 1);
    qmlRegisterUncreatableType<ActionTypeWrapper>(uri, 1, 0, "ActionType", QStringLiteral("You cannot create ActionType"));
    qmlRegisterSingletonType<DeclarativePersonPluginManager>(uri, 1, 0, "PersonPluginManager", [](QQmlEngine *, QJSEngine *) -> QObject * {
        return new DeclarativePersonPluginManager;
    });

    qmlRegisterUncreatableMetaObject(KPeople::staticMetaObject, uri, 1, 0, "KPeople", QStringLiteral("Access to enums & flags only"));
}

#include "peopleqmlplugin.moc"
