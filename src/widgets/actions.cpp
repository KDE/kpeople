/*
    SPDX-FileCopyrightText: 2015 Aleix Pol i Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "actions.h"
#include "../backends/abstractpersonaction.h"
#include "kpeople_widgets_debug.h"
#include <KPeople/PersonData>

#include <KPluginFactory>
#include <KPluginLoader>
#include <KPluginMetaData>

#include <QAction>

namespace KPeople
{
static QList<AbstractPersonAction *> actionsPlugins()
{
    QList<AbstractPersonAction *> actionPlugins;
    const QVector<KPluginMetaData> personPluginList = KPluginLoader::findPlugins(QStringLiteral("kpeople/actions"));
    for (const KPluginMetaData &service : personPluginList) {
        KPluginLoader loader(service.fileName());
        KPluginFactory *factory = loader.factory();
        if (!factory) {
            qCWarning(KPEOPLE_WIDGETS_LOG) << "Couldn't create the factory for" << service.name() << "at" << service.fileName();
            continue;
        }
        AbstractPersonAction *plugin = factory->create<AbstractPersonAction>();
        if (plugin) {
            //             qCDebug(KPEOPLE_WIDGETS_LOG) << "found plugin" << service->name();
            actionPlugins << plugin;
        }
    }

    return actionPlugins;
}

QList<QAction *> actionsForPerson(const QString &contactUri, QObject *parent)
{
    PersonData person(contactUri);

    QList<QAction *> actions;
    const auto lst = actionsPlugins();
    for (KPeople::AbstractPersonAction *plugin : lst) {
        actions << plugin->actionsForPerson(person, parent);
    }

    return actions;
}

}
