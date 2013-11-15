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

    QString genericAvatarImagePath;
    QList<AllContactsMonitorPtr> m_sourceMonitors;
};
}

using namespace KPeople;

PersonsModel::PersonsModel(QObject *parent):
    QAbstractItemModel(parent),
    d_ptr(new PersonsModelPrivate)
{
    Q_D(PersonsModel);

    d->genericAvatarImagePath = KStandardDirs::locate("data", "person-viewer/dummy_avatar.png");
    Q_FOREACH (BasePersonsDataSource* dataSource, PersonPluginManager::dataSourcePlugins()) {
        d->m_sourceMonitors << dataSource->allContactsMonitor();
    }

    onContactsFetched();

    Q_FOREACH(const AllContactsMonitorPtr monitor, d->m_sourceMonitors) {
        connect(monitor.data(), SIGNAL(contactAdded(QString,KABC::Addressee)), SLOT(onContactAdded(QString,KABC::Addressee)));
        connect(monitor.data(), SIGNAL(contactChanged(QString,KABC::Addressee)), SLOT(onContactChanged(QString,KABC::Addressee)));
        connect(monitor.data(), SIGNAL(contactRemoved(QString)), SLOT(onContactRemoved(QString)));
    }

    connect(PersonManager::instance(), SIGNAL(contactAddedToPerson(QString,QString)), SLOT(onAddContactToPerson(QString,QString)));
    connect(PersonManager::instance(), SIGNAL(contactRemovedFromPerson(QString)), SLOT(onRemoveContactsFromPerson(QString)));
}

PersonsModel::~PersonsModel()
{
}

QVariant PersonsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const PersonsModel);

    KABC::Addressee person;
    QString personId;

    if (index.parent().isValid()) {
        personId = d->personIds[index.parent().row()];
        person = d->metacontacts[personId].contacts().at(index.row());
    } else {
        personId = d->personIds[index.row()];
        person = d->metacontacts[personId].personAddressee();
    }

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
    }
    return QVariant();
}

int PersonsModel::columnCount(const QModelIndex &parent) const
{
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
    if (!parent.isValid()) {
        return createIndex(row, column, 0);
    }

    return createIndex(row, column, parent.row());
}

QModelIndex PersonsModel::parent(const QModelIndex &childIndex) const
{
    if (childIndex.internalId() == 0) {
        return QModelIndex();
    }

    return index(childIndex.internalId(), 0, QModelIndex());
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
        if (!addresseeMap.isEmpty()) {
            addPerson(MetaContact(key, contacts));
        }
    }

    //add remaining contacts
    KABC::Addressee::Map::const_iterator i;
    for (i = addresseeMap.constBegin(); i != addresseeMap.constEnd(); ++i) {
        addPerson(MetaContact(i.key(), i.value()));
    }

    emit modelInitialized();
}

void PersonsModel::onContactAdded(const QString &contactId, const KABC::Addressee &contact)
{
    Q_D(PersonsModel);

    const QString &personId = personIdForContact(contactId);

    if (d->personIds.contains(personId)) {
        int newContactPos = d->metacontacts[personId].contacts().size();
        //FIXME Dave has an idea about how to make this better
        beginInsertRows(index(d->personIds.indexOf(personId), 0), newContactPos, newContactPos);
        d->metacontacts[personId].updateContact(contactId, contact);
        endInsertRows();
        personChanged(personId);
    } else { //new contact -> new person
        addPerson(MetaContact(personId, contact));
    }
}

void PersonsModel::onContactChanged(const QString &contactId, const KABC::Addressee &contact)
{
    Q_D(PersonsModel);
    const QString &personId = personIdForContact(contactId);
    d->metacontacts[personId].updateContact(contactId, contact);

    const QModelIndex contactIndex = index(d->metacontacts[personId].contacts().indexOf(contactId),
                                           0,
                                           index(d->personIds.indexOf(personId), 0));

    Q_EMIT dataChanged(contactIndex, contactIndex);

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

void PersonsModel::onAddContactToPerson(const QString &contactId, const QString &newPersonId)
{
    Q_D(PersonsModel);

    const QString oldPersonId = personIdForContact(contactId);
    d->contactToPersons.insert(contactId, newPersonId);

    //get contact already in the model, remove it from the previous contact
    KABC::Addressee contact = d->metacontacts[oldPersonId].contact(contactId);
    int contactPosition = d->metacontacts[oldPersonId].contacts().indexOf(contact);
    beginRemoveRows(index(d->personIds.indexOf(oldPersonId), 0), contactPosition, contactPosition);
    d->metacontacts[oldPersonId].removeContact(contactId);
    endRemoveRows();

    if (!d->metacontacts[oldPersonId].isValid()) {
        removePerson(oldPersonId);
    } else {
        personChanged(oldPersonId);
    }

    //if the person is already in the model, add the contact to it
    if (d->personIds.contains(newPersonId)) {
        int newContactPos = d->metacontacts[newPersonId].contacts().size();
        beginInsertRows(index(d->personIds.indexOf(newPersonId), 0), newContactPos, newContactPos);
        d->metacontacts[newPersonId].updateContact(contactId, contact);
        endInsertRows();
    } else { //if the person is not in the model, create a new person and insert it
        KABC::Addressee::Map contacts;
        contacts[contactId] = contact;
        addPerson(MetaContact(newPersonId, contacts));
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

