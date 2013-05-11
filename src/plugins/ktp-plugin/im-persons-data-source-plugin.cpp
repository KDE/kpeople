#include <KPluginFactory>
#include <KPluginLoader>

#include "im-persons-data-source.h"

//this is a new file so that IMPluginDataSource can be used from it's own plugin (this) and from the normal plugin

K_PLUGIN_FACTORY( IMPersonsDataSourceFactory, registerPlugin<IMPersonsDataSource>(); )
K_EXPORT_PLUGIN( IMPersonsDataSourceFactory("im_persons_data_source_plugin") )
