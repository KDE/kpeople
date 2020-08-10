/*
    Persons Model
    SPDX-FileCopyrightText: 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef PEOPLEQMLPLUGIN_H
#define PEOPLEQMLPLUGIN_H

#include <QQmlExtensionPlugin>

class PeopleQMLPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void registerTypes(const char *uri) override;
};

#endif // PEOPLEQMLPLUGIN_H
