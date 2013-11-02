#include "personsmodel.h"

#include "personpluginmanager.h"
#include "metacontact.h"
#include "basepersonsdatasource.h"

#include <KABC/Addressee>

#include <QDebug>

namespace KPeople {
class PersonsModelPrivate{
public:
    QHash<QString /*Person ID*/, MetaContact> metacontacts;
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
        return d->metacontacts[id].personAddressee().name();
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
    QMultiHash<QString /*PersonID*/, QString /*ContactID*/> contactMapping;
    KABC::Addressee::Map addresseeMap;

    //temp test
    contactMapping.insertMulti("personA", "akonadi://?item=6743");
    contactMapping.insertMulti("personA", "akonadi://?item=6727");
    contactMapping.insertMulti("personA", "akonadi://?item=22");

    //temporary code, fetch data from all plugins
    KABC::AddresseeList contactList;
    foreach (BasePersonsDataSource* dataSource, PersonPluginManager::dataSourcePlugins()) {
        addresseeMap.unite(dataSource->allContacts());
    }

    //add metacontacts
    foreach (const QString &key, contactMapping.uniqueKeys()) {
        KABC::AddresseeList addressees;
        qDebug() << contactMapping.values(key);
        foreach (const QString &contact, contactMapping.values(key)) {
            addressees << addresseeMap.take(contact);
        }
        qDebug() << "adding contact " << key << addressees.size();
        addPerson(MetaContact(key, addressees));
    }

    qDebug() << addresseeMap.size();
    qDebug() << addresseeMap["akonadi://?item=6743"].isEmpty();

    //add remaining contacts
    foreach (const KABC::Addressee &addressee, addresseeMap.values()) {
        //FIXME proper iterator
        addPerson(MetaContact(addressee.uid(), KABC::AddresseeList() << addressee));
    }
}

void PersonsModel::addPerson(const KPeople::MetaContact& mc)
{
    Q_D(PersonsModel);

    const QString &id = mc.id();

    //TODO add to DataSourceWatcher (contactId -> personId)

    beginInsertRows(QModelIndex(), 0, d->personIds.size());
    d->metacontacts[id] = mc;
    d->personIds << id;
    endInsertRows();
}
