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
        KABC::Addressee::Map contacts;
        foreach (const QString &contact, contactMapping.values(key)) {
            if (addresseeMap.contains(contact)) {
                contacts[contact] = addresseeMap.take(contact);
            }
        }
        if (!addresseeMap.isEmpty()) {
            addPerson(MetaContact(key, contacts));
        }
    }

    //add remaining contacts
    KABC::Addressee::Map::const_iterator i;
    for (i = addresseeMap.constBegin(); i != addresseeMap.constEnd(); ++i) {
        addPerson(MetaContact(i.key(), i.value()));
    }
}

void PersonsModel::onContactAdded(const QString &contactId)
{
    Q_D(PersonsModel);
    //TODO map to personId properly
    const QString &personId = contactId;

    if (d->personIds.contains(personId)) {
        //TODO update the MC object for this person
        //dataChanged(..)
    } else { //new contact -> new person
        BasePersonsDataSource *dataSource = qobject_cast<BasePersonsDataSource*>(sender());

        //TODO async with an onContactFetched()
        const KABC::Addressee &contact = dataSource->contact(contactId);
        addPerson(MetaContact(personId, contact));
    }
}

void PersonsModel::onContactChanged(const QString &contactId)
{
    Q_D(PersonsModel);
    const QString &personId = contactId;

    //TODO async with an onContactFetched()
    BasePersonsDataSource *dataSource = qobject_cast<BasePersonsDataSource*>(sender());
    KABC::Addressee contact = dataSource->contact(contactId);

    d->metacontacts[personId].updateContact(contactId, contact);

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

    //TODO remove contact from MC object

    //TODO if MC object is now invalid remove the person from the list
    removePerson(personId);
}

void PersonsModel::onAddContactToPerson(const QString& contactId, const QString& personId)
{
    Q_D(PersonsModel);

    //get contact already in the model
    KABC::Addressee contact;
    removePerson(contactId);

    //if the person is already in the model, add the contact to it
    if (d->personIds.contains(personId)) {
        d->metacontacts[contactId].updateContact(contactId, contact);
    } else { //if the person is not in the model, create a new person and insert it
        addPerson(MetaContact(contactId, contact));
    }
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

