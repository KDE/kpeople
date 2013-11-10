#include "personsmodel.h"

#include "personpluginmanager.h"
#include "metacontact.h"
#include "basepersonsdatasource.h"
#include "personmanager.h"

#include <KABC/Addressee>

#include <QDebug>
#include <QPixmap>

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
    foreach (BasePersonsDataSource* dataSource, PersonPluginManager::dataSourcePlugins()) {
        connect(dataSource, SIGNAL(contactAdded(QString)), SLOT(onContactAdded(QString)));
        connect(dataSource, SIGNAL(contactChanged(QString)), SLOT(onContactChanged(QString)));
        connect(dataSource, SIGNAL(contactRemoved(QString)), SLOT(onContactRemoved(QString)));
    }

    onContactsFetched();
}

PersonsModel::~PersonsModel()
{
}

QVariant PersonsModel::data(const QModelIndex& index, int role) const
{
    Q_D(const PersonsModel);

    const QString &id = d->personIds[index.row()];
    const KABC::Addressee &person = d->metacontacts[id].personAddressee();
    if (role == Qt::DisplayRole) {
        return person.formattedName();
    } //TODO Hash<int, Field>
    if (role == Qt::DecorationRole) {
        if (!person.photo().data().isNull()) {
            return person.photo().data();
        } else {
            return QPixmap(person.photo().url());
        }
    }

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
    KABC::Addressee::Map addresseeMap;

    //temporary code, fetch data from all plugins
    KABC::AddresseeList contactList;
    foreach (BasePersonsDataSource* dataSource, PersonPluginManager::dataSourcePlugins()) {
        addresseeMap.unite(dataSource->allContacts());
    }

    //add metacontacts

    QMultiHash<QString, QString> contactMapping = PersonManager::instance()->allPersons();
    foreach (const QString &key, contactMapping.uniqueKeys()) {
        KABC::AddresseeList addressees;
        foreach (const QString &contact, contactMapping.values(key)) {
            if (addresseeMap.contains(contact)) {
                addressees << addresseeMap.take(contact);
            }
        }
        if (!addresseeMap.isEmpty()) {
            addPerson(MetaContact(key, addressees));
        }
    }

    //add remaining contacts
    KABC::Addressee::Map::const_iterator i;
    for (i = addresseeMap.constBegin(); i != addresseeMap.constEnd(); ++i) {
        addPerson(MetaContact(i.key(), KABC::AddresseeList() << i.value()));
    }
}

void PersonsModel::onContactAdded(const QString &contactId)
{
    qDebug() << "on contact added" << contactId;
    //TODO map to personId
    const QString &personId = contactId;

    BasePersonsDataSource *dataSource = qobject_cast<BasePersonsDataSource*>(sender());

    qDebug() << dataSource;

    //TODO async with an onContactFetched()
    const KABC::Addressee &contact = dataSource->contact(contactId);
    addPerson(MetaContact(personId, KABC::AddresseeList() << contact));
}

void PersonsModel::onContactChanged(const QString &contactId)
{
    Q_D(PersonsModel);
    const QString &personId = contactId;

    //TODO async with an onContactFetched()
    BasePersonsDataSource *dataSource = qobject_cast<BasePersonsDataSource*>(sender());
    KABC::Addressee contact = dataSource->contact(contactId);

    //FIXME this is broken
    //what if you had multiple contacts here
    //we need some sort of update(id, addressee)
    d->metacontacts[personId].updateContacts(KABC::AddresseeList() << contact);

    //mark as changed
    int row = d->personIds.indexOf(personId);
    const QModelIndex contactIndex = index(row);
    if(row >=0 ) {
        dataChanged(contactIndex, contactIndex);
    }
}

void PersonsModel::onContactRemoved(const QString &contactId)
{
    //TODO map to personId
    const QString &personId = contactId;
    removePerson(personId);
}



void PersonsModel::addPerson(const KPeople::MetaContact& mc)
{
    Q_D(PersonsModel);

    const QString &id = mc.id();

    //TODO add to DataSourceWatcher (contactId -> personId)

    beginInsertRows(QModelIndex(), d->personIds.size(), d->personIds.size());
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

