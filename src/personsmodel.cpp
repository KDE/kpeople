#include "personsmodel.h"

#include "personpluginmanager.h"
#include "metacontact.h"
#include "basepersonsdatasource.h"

#include <KABC/Addressee>

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>

namespace KPeople {
class PersonsModelPrivate{
public:
    //hash of person objects indexed by ID
    QHash<QString /*Person ID*/, MetaContact> metacontacts;
    //a list so we have an order in the model
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

    //load persons from DB
    //TODO make this a separate class whcih also does the dbus stuff

    //TODO create table if not exists persons (contactID varchar(255) unique not null, personID int);
    QSqlDatabase db= QSqlDatabase::addDatabase("QSQLITE3");
    db.setDatabaseName("/home/david/persondb");
    db.open();
    qDebug() << db.isOpen();

    QSqlQuery query = db.exec("SELECT personID, contactID FROM persons");
    while (query.next()) {
        const QString contactID = query.value(1).toString();
        const QString personId = "kpeople://" + query.value(0).toString(); // we store as ints internally, convert it to a string here
        contactMapping.insertMulti(personId, contactID);
    }

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

void PersonsModel::removePerson(const QString& id)
{
    Q_D(PersonsModel);

    int row = d->personIds.indexOf(id);
    if (row < 0) { //item not found
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    d->metacontacts.remove(id);
    d->personIds.removeOne(id);
    endRemoveRows();
}

