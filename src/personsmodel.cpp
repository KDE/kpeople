#include "personsmodel.h"

#include "personpluginmanager_p.h"
#include "metacontact_p.h"
#include "basepersonsdatasource.h"
#include "personmanager_p.h"

#include <KABC/Addressee>
#include <KStandardDirs>
#include <KDebug>

#include <QDebug>
#include <QPixmap>
#include <QTimer>

namespace KPeople {
class PersonsModelPrivate{
public:
    //NOTE This is the opposite way round to the return value from contactMapping() for easier lookups
    QHash<QString /*contactId*/, QString /*PersonId*/> contactToPersons;

    //hash of person objects indexed by ID
    QHash<QString /*Person ID*/, MetaContact> metacontacts;
    //a list so we have an order in the model
    QStringList personIds;

    QString genericAvatarImagePath;
    QList<AllContactsMonitorPtr> m_sourceMonitors;

    int initialFetchesDoneCount;
};
}

using namespace KPeople;

PersonsModel::PersonsModel(QObject *parent):
    QAbstractItemModel(parent),
    d_ptr(new PersonsModelPrivate)
{
    Q_D(PersonsModel);

    d->genericAvatarImagePath = KStandardDirs::locate("data", "kpeople/dummy_avatar.png");
    d->initialFetchesDoneCount = 0;
    Q_FOREACH (BasePersonsDataSource* dataSource, PersonPluginManager::dataSourcePlugins()) {
        const AllContactsMonitorPtr monitor = dataSource->allContactsMonitor();
        connect(monitor.data(), SIGNAL(initialFetchComplete()),
                this, SLOT(onMonitorInitialFetchComplete()));
        d->m_sourceMonitors << monitor;
    }

    connect(PersonManager::instance(), SIGNAL(contactAddedToPerson(QString,QString)), SLOT(onAddContactToPerson(QString,QString)));
    connect(PersonManager::instance(), SIGNAL(contactRemovedFromPerson(QString)), SLOT(onRemoveContactsFromPerson(QString)));
}

PersonsModel::~PersonsModel()
{
}

// QVariant dataForPerson(const KABC::Person &person)
// {
//
// }

QVariant PersonsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const PersonsModel);

    //optimization - if we don't cover this role, ignore it
    if (role < Qt::UserRole && role != Qt::DisplayRole && role != Qt::DecorationRole) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= rowCount(index.parent())) {
        return QVariant();
    }

    if (index.parent().isValid()) {
        if (role == ContactsVCardRole) {
            return QVariant::fromValue<KABC::AddresseeList>(KABC::AddresseeList());
        }
        const QString &personId = d->personIds[index.parent().row()];
        const MetaContact &mc = d->metacontacts[personId];

        return dataForAddressee(personId, mc.contacts().at(index.row()), role);
    } else {
        const QString &personId = d->personIds[index.row()];
        return dataForAddressee(personId, d->metacontacts[personId].personAddressee(), role);
    }
}

QVariant PersonsModel::dataForAddressee(const QString &personId, const KABC::Addressee &person, int role) const
{
    Q_D(const PersonsModel);

    switch(role) {
    case FormattedNameRole:
        return person.formattedName();
    case PhotoRole:
        if (!person.photo().data().isNull()) {
            return person.photo().data();
        } else if (!person.photo().url().isEmpty()) {
            return QImage(person.photo().url());
        } else {
            return QImage(d->genericAvatarImagePath);
        }
    case PersonIdRole:
        return personId;
    case PersonVCardRole:
        return QVariant::fromValue<KABC::Addressee>(person);
    case ContactsVCardRole:
        return QVariant::fromValue<KABC::AddresseeList>(d->metacontacts[personId].contacts());
    case GroupsRole:
        return person.categories();
    }
    return QVariant();
}

int PersonsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

int PersonsModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const PersonsModel);

    if (!parent.isValid()) {
        return d->personIds.size();
    }

    if (parent.isValid() && !parent.parent().isValid()) {
        return d->metacontacts[d->personIds.at(parent.row())].contacts().count();
    }

    return 0;
}

QModelIndex PersonsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0 || column < 0 || row >= rowCount(parent)) {
        return QModelIndex();
    }
    //top level items have internalId -1. Anything >=0 is the row of the top level item
    if (!parent.isValid()) {
        return createIndex(row, column, -1);
    }

    return createIndex(row, column, parent.row());
}

QModelIndex PersonsModel::parent(const QModelIndex &childIndex) const
{
    if (childIndex.internalId() == -1 || !childIndex.isValid()) {
        return QModelIndex();
    }

    return index(childIndex.internalId(), 0, QModelIndex());
}

void PersonsModel::onMonitorInitialFetchComplete()
{
    Q_D(PersonsModel);

    d->initialFetchesDoneCount++;
    Q_ASSERT(d->initialFetchesDoneCount <= d->m_sourceMonitors.count());
    if (d->initialFetchesDoneCount == d->m_sourceMonitors.count()) {

        onContactsFetched();
        Q_EMIT modelInitialized();
    }
}

void PersonsModel::onContactsFetched()
{
    Q_D(PersonsModel);
    KABC::Addressee::Map addresseeMap;

    //fetch all already loaded contacts from plugins
    KABC::AddresseeList contactList;
    Q_FOREACH (const AllContactsMonitorPtr &contactWatcher, d->m_sourceMonitors) {
        addresseeMap.unite(contactWatcher->contacts());
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
        if (!contacts.isEmpty()) {
            addPerson(MetaContact(key, contacts));
        }
    }

    //add remaining contacts
    KABC::Addressee::Map::const_iterator i;
    for (i = addresseeMap.constBegin(); i != addresseeMap.constEnd(); ++i) {
        addPerson(MetaContact(i.key(), i.value()));
    }

    Q_FOREACH(const AllContactsMonitorPtr monitor, d->m_sourceMonitors) {
        connect(monitor.data(), SIGNAL(contactAdded(QString,KABC::Addressee)), SLOT(onContactAdded(QString,KABC::Addressee)));
        connect(monitor.data(), SIGNAL(contactChanged(QString,KABC::Addressee)), SLOT(onContactChanged(QString,KABC::Addressee)));
        connect(monitor.data(), SIGNAL(contactRemoved(QString)), SLOT(onContactRemoved(QString)));
    }
}

void PersonsModel::onContactAdded(const QString &contactId, const KABC::Addressee &contact)
{
    Q_D(PersonsModel);

    const QString &personId = personIdForContact(contactId);

    if (d->personIds.contains(personId)) {
        MetaContact &mc = d->metacontacts[personId];

        //if the MC object already contains this object, we want to update the row, not do an insert
        if (mc.contactIds().contains(contactId)) {
            kWarning() << "Source emitted contactAdded for a contact we already know about " << contactId;
            onContactChanged(contactId, contact);
        } else {
            int newContactPos = mc.contacts().size();
            beginInsertRows(index(d->personIds.indexOf(personId)), newContactPos, newContactPos);
            mc.insertContact(contactId, contact);
            endInsertRows();
            personChanged(personId);
        }
    } else { //new contact -> new person
        KABC::Addressee::Map map;
        map[contactId] = contact;
        addPerson(MetaContact(personId, map));
    }
}

void PersonsModel::onContactChanged(const QString &contactId, const KABC::Addressee &contact)
{
    Q_D(PersonsModel);

    const QString &personId = personIdForContact(contactId);
    int row = d->metacontacts[personId].updateContact(contactId, contact);

    const QModelIndex contactIndex = index(row,
                                           0,
                                           index(d->personIds.indexOf(personId)));

    Q_EMIT dataChanged(contactIndex, contactIndex);

    personChanged(personId);
}

void PersonsModel::onContactRemoved(const QString &contactId)
{
    Q_D(PersonsModel);

    const QString &personId = personIdForContact(contactId);

    MetaContact &mc = d->metacontacts[personId];
    int contactPosition = d->metacontacts[personId].contactIds().indexOf(contactId);
    beginRemoveRows(index(d->personIds.indexOf(personId), 0), contactPosition, contactPosition);
    d->metacontacts[personId].removeContact(contactId);
    endRemoveRows();

    //if MC object is now invalid remove the person from the list
    if (!mc.isValid()) {
        removePerson(personId);
    }
    personChanged(personId);
}

void PersonsModel::onAddContactToPerson(const QString &contactId, const QString &newPersonId)
{
    Q_D(PersonsModel);

    const QString oldPersonId = personIdForContact(contactId);
    d->contactToPersons.insert(contactId, newPersonId);

    //get contact already in the model, remove it from the previous contact
    const KABC::Addressee &contact = d->metacontacts[oldPersonId].contact(contactId);
    int contactPosition = d->metacontacts[oldPersonId].contacts().indexOf(contact);
    beginRemoveRows(index(d->personIds.indexOf(oldPersonId), 0), contactPosition, contactPosition);
    d->metacontacts[oldPersonId].removeContact(contactId);
    endRemoveRows();

    if (!d->metacontacts[oldPersonId].isValid()) {
        removePerson(oldPersonId);
    } else {
        personChanged(oldPersonId);
    }

    //if the new person is already in the model, add the contact to it
    if (d->personIds.contains(newPersonId)) {
        int newContactPos = d->metacontacts[newPersonId].contacts().size();
        beginInsertRows(index(d->personIds.indexOf(newPersonId), 0), newContactPos, newContactPos);
        d->metacontacts[newPersonId].insertContact(contactId, contact);
        endInsertRows();
        personChanged(newPersonId);
    } else { //if the person is not in the model, create a new person and insert it
        KABC::Addressee::Map contacts;
        contacts[contactId] = contact;
        addPerson(MetaContact(newPersonId, contacts));
    }
}


void PersonsModel::onRemoveContactsFromPerson(const QString &contactId)
{
    Q_D(PersonsModel);

    const QString personId = personIdForContact(contactId);
    const KABC::Addressee &contact = d->metacontacts[personId].contact(contactId);
    d->metacontacts[personId].removeContact(contactId);
    d->contactToPersons.remove(contactId);

    //if we don't want the person object anymore
    if (!d->metacontacts[personId].isValid()) {
        removePerson(personId);
    } else {
        personChanged(personId);
    }

    //now re-insert as a new contact
    //we know it's not part of a metacontact anymore so reinsert as a contact
    addPerson(MetaContact(contactId, contact));
}

void PersonsModel::addPerson(const KPeople::MetaContact &mc)
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
    if (row >= 0) {
        const QModelIndex personIndex = index(row);
        dataChanged(personIndex, personIndex);
    }
}

QString PersonsModel::personIdForContact(const QString &contactId) const
{
    Q_D(const PersonsModel);
    //TODO optimize with constFind()
    if (d->contactToPersons.contains(contactId)) {
        return d->contactToPersons[contactId];
    } else {
        return contactId;
    }
}
