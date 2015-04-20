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
    QVector<KPluginMetaData> personPluginList = KPluginLoader::findPlugins(QStringLiteral("kpeople/actions"));
    Q_FOREACH (const KPluginMetaData &service, personPluginList) {
        KPluginLoader loader(service.fileName());
        KPluginFactory *factory = loader.factory();
        AbstractPersonAction *plugin = qobject_cast<AbstractPersonAction*>(factory->create());
        if (plugin) {
//             qDebug() << "found plugin" << service->name();
            actionPlugins << plugin;
        }
    }

    return actionPlugins;
}

QList<QAction *> actionsForPerson(const QString &contactUri, QObject *parent)
{
    PersonData person(contactUri);

    QList<QAction *> actions;
    Q_FOREACH (KPeople::AbstractPersonAction *plugin, actionsPlugins()) {
        actions << plugin->actionsForPerson(person, parent);
    }

    return actions;
}

}
