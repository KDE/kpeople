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
#include "backends/basepersonsdatasource.h"

#include <KService>
#include <KServiceTypeTrader>
#include <KPluginInfo>

#include <QMutex>
#include <QDebug>

using namespace KPeople;

class PersonPluginManagerPrivate
{
public:
    PersonPluginManagerPrivate();
    ~PersonPluginManagerPrivate();
    QHash<QString /* SourceName*/, BasePersonsDataSource *> dataSourcePlugins;

    void loadDataSourcePlugins();
    bool m_loadedDataSourcePlugins;
    QMutex m_mutex;

};

Q_GLOBAL_STATIC(PersonPluginManagerPrivate, s_instance);

PersonPluginManagerPrivate::PersonPluginManagerPrivate():
    m_loadedDataSourcePlugins(false)
{
}

PersonPluginManagerPrivate::~PersonPluginManagerPrivate()
{
    qDeleteAll(dataSourcePlugins);
}

void PersonPluginManagerPrivate::loadDataSourcePlugins()
{
    KService::List pluginList = KServiceTypeTrader::self()->query(QLatin1String("KPeople/DataSource"));
    Q_FOREACH (const KService::Ptr &service, pluginList) {
        BasePersonsDataSource *dataSource = service->createInstance<BasePersonsDataSource>(0);
        if (dataSource) {
            dataSourcePlugins[dataSource->sourcePluginId()] = dataSource;
        } else {
            qWarning() << "Failed to create data source " << service->name() << service->path();
        }
    }
    m_loadedDataSourcePlugins = true;
}

void PersonPluginManager::setDataSourcePlugins(const QHash<QString, BasePersonsDataSource * > &dataSources)
{
    s_instance->m_mutex.lock();
    qDeleteAll(s_instance->dataSourcePlugins);
    s_instance->dataSourcePlugins = dataSources;
    s_instance->m_loadedDataSourcePlugins = true;
    s_instance->m_mutex.unlock();
}

QList<BasePersonsDataSource *> PersonPluginManager::dataSourcePlugins()
{
    s_instance->m_mutex.lock();
    if (!s_instance->m_loadedDataSourcePlugins) {
        s_instance->loadDataSourcePlugins();
    }
    s_instance->m_mutex.unlock();
    return s_instance->dataSourcePlugins.values();
}

BasePersonsDataSource *PersonPluginManager::dataSource(const QString &sourceId)
{
    s_instance->m_mutex.lock();
    if (!s_instance->m_loadedDataSourcePlugins) {
        s_instance->loadDataSourcePlugins();
    }
    s_instance->m_mutex.unlock();

    return s_instance->dataSourcePlugins.value(sourceId);
}
