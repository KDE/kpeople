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


#include "personpluginmanager_p.h"
#include "basepersonsdatasource.h"

#include "abstractpersonaction.h"

#include <KService>
#include <KServiceTypeTrader>
#include <KPluginInfo>
#include <KDebug>

#include <QMutex>

#include <kdemacros.h>

using namespace KPeople;

class PersonPluginManagerPrivate
{
public:
    PersonPluginManagerPrivate();
    ~PersonPluginManagerPrivate();
    QList<AbstractPersonAction*> actionPlugins;
    QHash<QString /* SourceName*/, BasePersonsDataSource*> dataSourcePlugins;

    void loadDataSourcePlugins();
    void loadActionsPlugins();
    bool m_loadedDataSourcePlugins;
    bool m_loadedActionsPlugins;
    QMutex m_mutex;

};

K_GLOBAL_STATIC(PersonPluginManagerPrivate, s_instance);

PersonPluginManagerPrivate::PersonPluginManagerPrivate():
    m_loadedDataSourcePlugins(false),
    m_loadedActionsPlugins(false)
{
}

PersonPluginManagerPrivate::~PersonPluginManagerPrivate()
{
    qDeleteAll(dataSourcePlugins);
    qDeleteAll(actionPlugins);
}

void PersonPluginManagerPrivate::loadDataSourcePlugins()
{
    KService::List pluginList = KServiceTypeTrader::self()->query(QLatin1String("KPeople/DataSource"));
    Q_FOREACH(const KService::Ptr &service, pluginList) {
        BasePersonsDataSource* dataSource = service->createInstance<BasePersonsDataSource>(0);
        if (dataSource) {
            dataSourcePlugins[dataSource->sourcePluginId()] = dataSource;
        } else {
            kWarning() << "Failed to create data source " << service->name() << service->path();
        }
    }
    m_loadedDataSourcePlugins = true;
}

void PersonPluginManagerPrivate::loadActionsPlugins()
{
    KService::List personPluginList = KServiceTypeTrader::self()->query(QLatin1String("KPeople/Plugin"));
    Q_FOREACH(const KService::Ptr &service, personPluginList) {
        AbstractPersonAction *plugin = service->createInstance<AbstractPersonAction>(0);
        if (plugin) {
            qDebug() << "found plugin" << service->name();
            actionPlugins << plugin;
        }
    }
    m_loadedActionsPlugins = true;
}

void PersonPluginManager::setDataSourcePlugins(const QHash<QString, BasePersonsDataSource* > &dataSources)
{
    s_instance->m_mutex.lock();
    qDeleteAll(s_instance->dataSourcePlugins);
    s_instance->dataSourcePlugins.clear();
    s_instance->dataSourcePlugins = dataSources;
    s_instance->m_loadedDataSourcePlugins = true;
    s_instance->m_mutex.unlock();
}

QList<BasePersonsDataSource*> PersonPluginManager::dataSourcePlugins()
{
    s_instance->m_mutex.lock();
    if (!s_instance->m_loadedDataSourcePlugins) {
        s_instance->loadDataSourcePlugins();
    }
    s_instance->m_mutex.unlock();
    return s_instance->dataSourcePlugins.values();
}

BasePersonsDataSource* PersonPluginManager::dataSource(const QString &sourceId)
{
    s_instance->m_mutex.lock();
    if (!s_instance->m_loadedDataSourcePlugins) {
        s_instance->loadDataSourcePlugins();
    }
    s_instance->m_mutex.unlock();

    return s_instance->dataSourcePlugins[sourceId];
}

QList<AbstractPersonAction*> PersonPluginManager::actions()
{
    s_instance->m_mutex.lock();
    if (!s_instance->m_loadedActionsPlugins) {
        s_instance->loadActionsPlugins();
    }
    s_instance->m_mutex.unlock();

    return s_instance->actionPlugins;
}
