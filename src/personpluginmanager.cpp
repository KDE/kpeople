/*
    SPDX-FileCopyrightText: 2013 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "personpluginmanager.h"
#include "backends/basepersonsdatasource.h"

#include "kpeople_debug.h"

#include <KPluginFactory>
#include <KPluginLoader>
#include <KPluginMetaData>

#include <QMutex>
#include <QMutexLocker>

using namespace KPeople;

class PersonPluginManagerPrivate
{
public:
    PersonPluginManagerPrivate();
    ~PersonPluginManagerPrivate();
    QHash<QString /* SourceName*/, BasePersonsDataSource *> dataSourcePlugins;

    void loadDataSourcePlugins();
    bool m_autoloadDataSourcePlugins = true;
    bool m_loadedDataSourcePlugins = false;
    QMutex m_mutex;
};

Q_GLOBAL_STATIC(PersonPluginManagerPrivate, s_instance)

PersonPluginManagerPrivate::PersonPluginManagerPrivate()
{
}

PersonPluginManagerPrivate::~PersonPluginManagerPrivate()
{
    qDeleteAll(dataSourcePlugins);
}

void PersonPluginManagerPrivate::loadDataSourcePlugins()
{
    const QVector<KPluginMetaData> pluginList = KPluginLoader::findPlugins(QStringLiteral("kpeople/datasource"));
    for (const KPluginMetaData &service : pluginList) {
        KPluginLoader loader(service.fileName());
        KPluginFactory *factory = loader.factory();
        BasePersonsDataSource *dataSource = factory->create<BasePersonsDataSource>();
        if (dataSource) {
            const QString pluginId = dataSource->sourcePluginId();
            if (!dataSourcePlugins.contains(pluginId)) {
                dataSourcePlugins[pluginId] = dataSource;
            } else {
                dataSource->deleteLater();
                qCDebug(KPEOPLE_LOG) << "Plugin" << pluginId << "was already loaded manually, ignoring...";
            }
        } else {
            qCWarning(KPEOPLE_LOG) << "Failed to create data source " << service.name() << service.fileName();
        }
    }

    m_loadedDataSourcePlugins = true;
}

void PersonPluginManager::setAutoloadDataSourcePlugins(bool autoloadDataSourcePlugins)
{
    s_instance->m_autoloadDataSourcePlugins = autoloadDataSourcePlugins;
}

void PersonPluginManager::addDataSource(const QString &sourceId, BasePersonsDataSource *source)
{
    QMutexLocker(&s_instance->m_mutex);
    if (s_instance->dataSourcePlugins.contains(sourceId)) {
        qCWarning(KPEOPLE_LOG) << "Attempting to load data source that is already loaded, overriding!";
        s_instance->dataSourcePlugins[sourceId]->deleteLater();
    }
    s_instance->dataSourcePlugins.insert(sourceId, source);
}

void PersonPluginManager::setDataSourcePlugins(const QHash<QString, BasePersonsDataSource *> &dataSources)
{
    QMutexLocker(&s_instance->m_mutex);
    qDeleteAll(s_instance->dataSourcePlugins);
    s_instance->dataSourcePlugins = dataSources;
    s_instance->m_loadedDataSourcePlugins = true;
}

QList<BasePersonsDataSource *> PersonPluginManager::dataSourcePlugins()
{
    QMutexLocker(&s_instance->m_mutex);
    if (!s_instance->m_loadedDataSourcePlugins && s_instance->m_autoloadDataSourcePlugins) {
        s_instance->loadDataSourcePlugins();
    }
    return s_instance->dataSourcePlugins.values();
}

BasePersonsDataSource *PersonPluginManager::dataSource(const QString &sourceId)
{
    QMutexLocker(&s_instance->m_mutex);
    if (!s_instance->m_loadedDataSourcePlugins && s_instance->m_autoloadDataSourcePlugins) {
        s_instance->loadDataSourcePlugins();
    }
    return s_instance->dataSourcePlugins.value(sourceId);
}

bool KPeople::PersonPluginManager::addContact(const QVariantMap &properties)
{
    bool ret = false;
    for (auto p : std::as_const(s_instance->dataSourcePlugins)) {
        auto v2 = dynamic_cast<BasePersonsDataSourceV2 *>(p);
        if (!v2) {
            continue;
        }
        const bool added = v2->addContact(properties);
        ret |= added;
    }
    return ret;
}

bool KPeople::PersonPluginManager::deleteContact(const QString &uri)
{
    bool ret = false;
    for (auto p : std::as_const(s_instance->dataSourcePlugins)) {
        auto v2 = dynamic_cast<BasePersonsDataSourceV2 *>(p);
        if (!v2) {
            continue;
        }
        const bool deleted = v2->deleteContact(uri);
        ret |= deleted;
    }
    return ret;
}
