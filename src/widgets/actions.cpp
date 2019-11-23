/*
    Copyright (C) 2015 Aleix Pol i Gonzalez <aleixpol@blue-systems.com>

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

#include "actions.h"
#include <QAction>
#include "kpeople_widgets_debug.h"
#include <KPluginLoader>
#include <KPluginFactory>
#include <KPluginMetaData>
#include <KPeople/PersonData>
#include "../backends/abstractpersonaction.h"

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
