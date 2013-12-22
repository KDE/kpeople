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


#include "personpluginmanager.h"
#include "basepersonsdatasource.h"

#include "abstractpersonplugin.h"

#include <KService>
#include <KServiceTypeTrader>
#include <KPluginInfo>
#include <KDebug>

#include <kdemacros.h>

using namespace KPeople;

class PersonPluginManagerPrivate
{
public:
    PersonPluginManagerPrivate();
    ~PersonPluginManagerPrivate();
    QList<AbstractPersonPlugin*> personPlugins;
    QList<BasePersonsDataSource*> dataSourcePlugins;
};

K_GLOBAL_STATIC(PersonPluginManagerPrivate, s_instance);

PersonPluginManagerPrivate::PersonPluginManagerPrivate()
{
    KService::List pluginList = KServiceTypeTrader::self()->query(QLatin1String("KPeople/DataSource"));
    Q_FOREACH(const KService::Ptr &service, pluginList) {
        BasePersonsDataSource* dataSource = service->createInstance<BasePersonsDataSource>(0);
        if (dataSource) {
            qDebug() << "adding  **** " << dataSource << service->name();
            dataSourcePlugins << dataSource;
        } else {
            kWarning() << "Failed to create data source";
        }
    }

    KService::List personPluginList = KServiceTypeTrader::self()->query(QLatin1String("KPeople/Plugin"));
    Q_FOREACH(const KService::Ptr &service, personPluginList) {
        AbstractPersonPlugin *plugin = service->createInstance<AbstractPersonPlugin>(0);
        if (plugin) {
            qDebug() << "found plugin" << service->name();
            personPlugins << plugin;
        }
    }
}

PersonPluginManagerPrivate::~PersonPluginManagerPrivate()
{
    qDeleteAll(dataSourcePlugins);
    qDeleteAll(personPlugins);
}


void PersonPluginManager::setDataSourcePlugins(const QList< BasePersonsDataSource* >& dataSources)
{
    qDeleteAll(s_instance->dataSourcePlugins);
    s_instance->dataSourcePlugins.clear();
    s_instance->dataSourcePlugins = dataSources;
}

QList<BasePersonsDataSource*> PersonPluginManager::dataSourcePlugins()
{
    return s_instance->dataSourcePlugins;
}

QList<AbstractPersonPlugin*> PersonPluginManager::personPlugins()
{
    return s_instance->personPlugins;
}
