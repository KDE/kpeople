#include "personsmodel.h"

#include "personpluginmanager.h"
#include "metacontact.h"
#include "basepersonsdatasource.h"

namespace KPeople {
class PersonsModelPrivate{
public:
    QMap<QString /*Person ID*/, MetaContact> metacontacts;
    QStringList personIds;
};
}

using namespace KPeople;

PersonsModel::PersonsModel(QObject *parent):
    QAbstractListModel(parent),
    d_ptr(new PersonsModelPrivate)
{
    onContactsFetched();
}

PersonsModel::~PersonsModel()
{
}

QVariant PersonsModel::data(const QModelIndex& index, int role) const
{
    Q_D(const PersonsModel);

    const QString &id = d->personIds[index.row()];
    if (role == Qt::DisplayRole) {
        return d->metacontacts[id].personAddressee().formattedName();
    } //TODO Hash<int, Field>

    return QVariant();
}

int PersonsModel::rowCount(const QModelIndex& parent) const
{
    Q_D(const PersonsModel);

    if (parent.isValid()) {
        return 0;
    }
    return d->personIds.size();
}

void PersonsModel::onContactsFetched() //TODO async this
{
    Q_D(PersonsModel);

    KABC::AddresseeList contactList;
    foreach (BasePersonsDataSource* dataSource, PersonPluginManager::dataSourcePlugins()) {
        contactList << dataSource->allContacts();
    }

    beginInsertRows(QModelIndex(), 0, contactList.size());
    foreach (const KABC::Addressee &addressee, contactList) {
        d->metacontacts[addressee.uid()] = MetaContact(KABC::AddresseeList() << addressee);
    }
    endInsertRows();
}
