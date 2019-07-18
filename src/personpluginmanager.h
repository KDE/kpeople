/*
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>
    Copyright (C) 2018  Igor Poboiko <igor.poboiko@gmail.com>

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

#ifndef PERSON_PLUGIN_MANAGER_H
#define PERSON_PLUGIN_MANAGER_H


#include <kpeople/kpeople_export.h>

#include <QHash>

namespace KPeople
{

class AbstractPersonAction;
class BasePersonsDataSource;

/**
  This class allows applications to manage DataSource plugins

  It can be useful if an application wants to use custom DataSource,
  without exposing it to other KPeople-based applications
  (i.e. without installing a system-wide plugin)

  Another use-case is that it allows to pass custom arguments to DataSources
  (i.e. an ItemModel which will be used as a source of data)

  @since 5.51
 */
class KPEOPLE_EXPORT PersonPluginManager
{
public:
    /**
     * Use this if you explicitly don't want KPeople to autoload all the
     * available data source plugins via KPluginLoader.
     *
     * The default behavior is to autoload them
     */
    static void setAutoloadDataSourcePlugins(bool autoloadDataSourcePlugins);
    static QList<BasePersonsDataSource *> dataSourcePlugins();
    /**
     * Adds custom data source. If DataSource with such @p sourceId was already loaded, we override it
     *
     * Takes ownership of the @p source
     */
    static void addDataSource(const QString &sourceId, BasePersonsDataSource *source);
    static BasePersonsDataSource *dataSource(const QString &sourceId);
    static QList<AbstractPersonAction *> actions();

    /**
     * Instead of loading datasources from plugins, set sources manually
     * This is for unit tests only
     */
    static void setDataSourcePlugins(const QHash<QString, BasePersonsDataSource *> &dataSources);
};
}

#endif // PERSON_PLUGIN_MANAGER_H
