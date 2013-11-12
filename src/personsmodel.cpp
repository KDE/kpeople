#include "personsmodel.h"

#include "personpluginmanager.h"
#include "metacontact.h"
#include "basepersonsdatasource.h"
#include "personmanager.h"

#include <KABC/Addressee>
#include <KStandardDirs>

#include <QDebug>
#include <QPixmap>

namespace KPeople {
class PersonsModelPrivate{
public:
    //NOTE This is the opposite way round to the return value from contactMapping() for easier lookups
    QHash<QString /*contactId*/, QString /*PersonId*/> contactToPersons;

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

    Q_FOREACH (BasePersonsDataSource* dataSource, PersonPluginManager::dataSourcePlugins()) {
        connect(dataSource, SIGNAL(contactAdded(QString)), SLOT(onContactAdded(QString)));
        connect(dataSource, SIGNAL(contactChanged(QString)), SLOT(onContactChanged(QString)));
        connect(dataSource, SIGNAL(contactRemoved(QString)), SLOT(onContactRemoved(QString)));
    }

    connect(PersonManager::instance(), SIGNAL(contactAddedToPerson(QString,QString)), SLOT(onAddContactToPerson(QString,QString)));
    connect(PersonManager::instance(), SIGNAL(contactRemovedFromPerson(QString)), SLOT(onRemoveContactsFromPerson(QString)));
}

PersonsModel::~PersonsModel()
{
}

QVariant PersonsModel::data(const QModelIndex& index, int role) const
{
    Q_D(const PersonsModel);

    const QString &personId = d->personIds[index.row()];
    const KABC::Addressee &person = d->metacontacts[personId].personAddressee();

    switch(role) {
    case FormattedNameRole:
            return person.formattedName();
    case PhotoRole:
        if (!person.photo().data().isNull()) {
            return person.photo().data();
        } else if (!person.photo().url().isEmpty()) {
            return QImage(person.photo().url());
        } else {
            const QString avatarImagePath = KStandardDirs::locate("data", "person-viewer/dummy_avatar.png");
            return QImage(avatarImagePath);
        }
    case PersonIdRole:
        return personId;
    case PersonVCardRole:
        return QVariant::fromValue<KABC::Addressee>(person);
    case ContactsVCardRole:
        return QVariant::fromValue<KABC::AddresseeList>(d->metacontacts[personId].contacts());
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
    Q_D(PersonsModel);
    KABC::Addressee::Map addresseeMap;

    //temporary code, fetch data from all plugins
    KABC::AddresseeList contactList;
    Q_FOREACH (BasePersonsDataSource* dataSource, PersonPluginManager::dataSourcePlugins()) {
        addresseeMap.unite(dataSource->allContacts());
    }

    //add metacontacts

    QMultiHash<QString, QString> contactMapping = PersonManager::instance()->allPersons();

    Q_FOREACH (const QString &key, contactMapping.uniqueKeys()) {
        KABC::Addressee::Map contacts;
        Q_FOREACH (const QString &contact, contactMapping.values(key)) {
            d->contactToPersons[contact] = key;
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

    BasePersonsDataSource *dataSource = qobject_cast<BasePersonsDataSource*>(sender());

    //TODO async with an onContactFetched()
    const KABC::Addressee &contact = dataSource->contact(contactId);

    const QString &personId = personIdForContact(contactId);

    if (d->personIds.contains(personId)) {
        d->metacontacts[personId].updateContact(contactId, contact);
        personChanged(personId);
    } else { //new contact -> new person
        addPerson(MetaContact(personId, contact));
    }
}

void PersonsModel::onContactChanged(const QString &contactId)
{
    Q_D(PersonsModel);
    const QString &personId = personIdForContact(contactId);

    //TODO async with an onContactFetched()
    BasePersonsDataSource *dataSource = qobject_cast<BasePersonsDataSource*>(sender());
    KABC::Addressee contact = dataSource->contact(contactId);

    d->metacontacts[personId].updateContact(contactId, contact);

    personChanged(personId);
}

void PersonsModel::onContactRemoved(const QString &contactId)
{
    Q_D(PersonsModel);

    const QString &personId = personIdForContact(contactId);

    MetaContact &mc = d->metacontacts[personId];
    mc.removeContact(personId);

    //if MC object is now invalid remove the person from the list
    if (!mc.isValid()) {
        removePerson(personId);
    }
}

void PersonsModel::onAddContactToPerson(const QString& contactId, const QString& newPersonId)
{
    Q_D(PersonsModel);

    const QString oldPersonId = personIdForContact(contactId);
    d->contactToPersons.insert(contactId, newPersonId);

    //get contact already in the model, remove it from the previous contact
    KABC::Addressee contact = d->metacontacts[oldPersonId].contact(contactId);
    d->metacontacts[oldPersonId].removeContact(contactId);
    if (!d->metacontacts[oldPersonId].isValid()) {
        removePerson(oldPersonId);
    } else {
        personChanged(oldPersonId);
    }

    //if the person is already in the model, add the contact to it
    if (d->personIds.contains(newPersonId)) {
        d->metacontacts[newPersonId].updateContact(contactId, contact);
    } else { //if the person is not in the model, create a new person and insert it
        addPerson(MetaContact(newPersonId, contact));
    }
}

void PersonsModel::onRemoveContactsFromPerson(const QString& contactId)
{
    Q_D(PersonsModel);

    const QString personId = personIdForContact(contactId);
    const KABC::Addressee contact = d->metacontacts[personId].contact(contactId);
    d->metacontacts[personId].removeContact(contactId);
    d->contactToPersons.remove(contactId);

    //if we don't want the person object anymore
    if (!d->metacontacts[personId].isValid()) {
        removePerson(personId);
    }

    //now re-insert as a new contact
    //we know it's not part of a metacontact anymore so reinsert as a contact
    addPerson(MetaContact(contactId, contact));
}

void PersonsModel::addPerson(const KPeople::MetaContact& mc)
{
    Q_D(PersonsModel);

    const QString &id = mc.id();

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

void PersonsModel::personChanged(const QString &personId)
{
    Q_D(const PersonsModel);
    int row = d->personIds.indexOf(personId);
    const QModelIndex contactIndex = index(row);
    if(row >=0 ) {
        dataChanged(contactIndex, contactIndex);
    }
}

QString PersonsModel::personIdForContact(const QString& contactId)
{
    Q_D(const PersonsModel);
    //TODO optimise with constFind()
    if (d->contactToPersons.contains(contactId)) {
        return d->contactToPersons[contactId];
    } else {
        return contactId;
    }
}

