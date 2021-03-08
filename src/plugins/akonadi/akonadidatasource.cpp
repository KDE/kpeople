/*
    SPDX-FileCopyrightText: 2013 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "akonadidatasource.h"

#include <Akonadi/Collection>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>
#include <Akonadi/Item>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/ServerManager>

#include <KPluginFactory>
#include <KPluginLoader>

#include "kpeople_akonadi_plugin_debug.h"

using namespace Akonadi;

class AkonadiAllContacts : public KPeople::AllContactsMonitor
{
    Q_OBJECT
public:
    AkonadiAllContacts();
    ~AkonadiAllContacts();
    virtual QMap<QString, AbstractContact::Ptr> contacts();
private Q_SLOTS:
    void onCollectionsFetched(KJob *job);
    void onItemsFetched(KJob *job);
    void onItemAdded(const Akonadi::Item &item);
    void onItemChanged(const Akonadi::Item &item);
    void onItemRemoved(const Akonadi::Item &item);
    void onServerStateChanged(Akonadi::ServerManager::State);

private:
    Akonadi::Monitor *m_monitor;
    QMap<QString, AbstractContact::Ptr> m_contacts;
    int m_activeFetchJobsCount;
    bool m_fetchError;
};

AkonadiAllContacts::AkonadiAllContacts()
    : m_monitor(new Akonadi::Monitor(this))
    , m_activeFetchJobsCount(0)
    , m_fetchError(false)
{
    connect(Akonadi::ServerManager::self(), SIGNAL(stateChanged(Akonadi::ServerManager::State)), SLOT(onServerStateChanged(Akonadi::ServerManager::State)));
    onServerStateChanged(Akonadi::ServerManager::state());

    connect(m_monitor, SIGNAL(itemAdded(Akonadi::Item, Akonadi::Collection)), SLOT(onItemAdded(Akonadi::Item)));
    connect(m_monitor, SIGNAL(itemChanged(Akonadi::Item, QSet<QByteArray>)), SLOT(onItemChanged(Akonadi::Item)));
    connect(m_monitor, SIGNAL(itemRemoved(Akonadi::Item)), SLOT(onItemRemoved(Akonadi::Item)));

    m_monitor->setMimeTypeMonitored("text/directory");
    m_monitor->itemFetchScope().fetchFullPayload();
    m_monitor->itemFetchScope().setFetchModificationTime(false);
#ifdef HAVE_KDEPIM_ATLEAST_412
    m_monitor->itemFetchScope().setFetchRemoteIdentification(false);
#endif

    CollectionFetchJob *fetchJob = new CollectionFetchJob(Collection::root(), CollectionFetchJob::Recursive, this);
    fetchJob->fetchScope().setContentMimeTypes(QStringList() << "text/directory");
    connect(fetchJob, SIGNAL(finished(KJob *)), SLOT(onCollectionsFetched(KJob *)));
}

AkonadiAllContacts::~AkonadiAllContacts()
{
}

QMap<QString, AbstractContact::Ptr> AkonadiAllContacts::contacts()
{
    return m_contacts;
}

QString AkonadiDataSource::sourcePluginId() const
{
    return "akonadi";
}

void AkonadiAllContacts::onItemAdded(const Item &item)
{
    if (!item.hasPayload<AbstractContact::Ptr>()) {
        return;
    }
    const QString id = item.url().prettyUrl();
    const AbstractContact::Ptr contact = item.payload<AbstractContact::Ptr>();
    m_contacts[id] = contact;
    Q_EMIT contactAdded(item.url().prettyUrl(), contact);
}

void AkonadiAllContacts::onItemChanged(const Item &item)
{
    if (!item.hasPayload<AbstractContact::Ptr>()) {
        return;
    }
    const QString id = item.url().prettyUrl();
    const AbstractContact::Ptr contact = item.payload<AbstractContact::Ptr>();
    m_contacts[id] = contact;
    Q_EMIT contactChanged(item.url().prettyUrl(), contact);
}

void AkonadiAllContacts::onItemRemoved(const Item &item)
{
    if (!item.hasPayload<AbstractContact::Ptr>()) {
        return;
    }
    const QString id = item.url().prettyUrl();
    m_contacts.remove(id);
    Q_EMIT contactRemoved(id);
}

// or we could add items as we go along...
void AkonadiAllContacts::onItemsFetched(KJob *job)
{
    if (job->error()) {
        qCWarning(KPEOPLE_AKONADI_PLUGIN_LOG) << job->errorString();
        m_fetchError = true;
    } else {
        ItemFetchJob *itemFetchJob = qobject_cast<ItemFetchJob *>(job);
        foreach (const Item &item, itemFetchJob->items()) {
            onItemAdded(item);
        }
    }

    if (--m_activeFetchJobsCount == 0 && !isInitialFetchComplete()) {
        emitInitialFetchComplete(true);
    }
}

void AkonadiAllContacts::onCollectionsFetched(KJob *job)
{
    if (job->error()) {
        qCWarning(KPEOPLE_AKONADI_PLUGIN_LOG) << job->errorString();
        emitInitialFetchComplete(false);
    } else {
        CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob *>(job);
        QList<Collection> contactCollections;
        foreach (const Collection &collection, fetchJob->collections()) {
            // Skip virtual collections - we will get contacts linked into virtual
            // collections from their real parent collections
            if (collection.isVirtual()) {
                continue;
            }
            if (collection.contentMimeTypes().contains(AbstractContact::Ptr::mimeType())) {
                ItemFetchJob *itemFetchJob = new ItemFetchJob(collection);
                itemFetchJob->fetchScope().fetchFullPayload();
                connect(itemFetchJob, SIGNAL(finished(KJob *)), SLOT(onItemsFetched(KJob *)));
                ++m_activeFetchJobsCount;
            }
        }
        if (m_activeFetchJobsCount == 0) {
            emitInitialFetchComplete(true);
        }
    }
    if (m_activeFetchJobsCount == 0 && !isInitialFetchComplete()) {
        emitInitialFetchComplete(true);
    }
}

void AkonadiAllContacts::onServerStateChanged(ServerManager::State state)
{
    // if we're broken tell kpeople we've loaded so kpeople doesn't block
    if (state == Akonadi::ServerManager::Broken && !isInitialFetchComplete()) {
        emitInitialFetchComplete(false);
        qCWarning(KPEOPLE_AKONADI_PLUGIN_LOG) << "Akonadi failed to load, some metacontact features may not be available";
        qCWarning(KPEOPLE_AKONADI_PLUGIN_LOG) << "For more information please load akonadi_console";
    }
}

class AkonadiContact : public KPeople::ContactMonitor
{
    Q_OBJECT
public:
    AkonadiContact(Akonadi::Monitor *monitor, const QString &contactUri);
    ~AkonadiContact();
private Q_SLOTS:
    void onContactFetched(KJob *);
    void onContactChanged(const Akonadi::Item &);

private:
    Akonadi::Monitor *m_monitor;
    Akonadi::Item m_item;
};

AkonadiContact::AkonadiContact(Akonadi::Monitor *monitor, const QString &contactUri)
    : ContactMonitor(contactUri)
    , m_monitor(monitor)
{
    // TODO: optimiZation, base class could copy across from the model if the model exists
    // then we should check if contact is already set to something and avoid the initial fetch

    // load the contact initially
    m_item = Item::fromUrl(QUrl(contactUri));
    ItemFetchJob *itemFetchJob = new ItemFetchJob(m_item);
    itemFetchJob->fetchScope().fetchFullPayload();
    connect(itemFetchJob, SIGNAL(finished(KJob *)), SLOT(onContactFetched(KJob *)));

    // then watch for that item changing
    m_monitor->setItemMonitored(m_item, true);
    connect(m_monitor, SIGNAL(itemChanged(Akonadi::Item, QSet<QByteArray>)), SLOT(onContactChanged(Akonadi::Item)));
}

AkonadiContact::~AkonadiContact()
{
    m_monitor->setItemMonitored(m_item, false);
}

void AkonadiContact::onContactFetched(KJob *job)
{
    ItemFetchJob *fetchJob = qobject_cast<ItemFetchJob *>(job);
    if (fetchJob->items().count() && fetchJob->items().first().hasPayload<AbstractContact::Ptr>()) {
        setContact(fetchJob->items().first().payload<AbstractContact::Ptr>());
    }
}

void AkonadiContact::onContactChanged(const Item &item)
{
    if (item != m_item) {
        return;
    }
    if (!item.hasPayload<AbstractContact::Ptr>()) {
        return;
    }
    setContact(item.payload<AbstractContact::Ptr>());
}

AkonadiDataSource::AkonadiDataSource(QObject *parent, const QVariantList &args)
    : BasePersonsDataSource(parent)
    , m_monitor(new Akonadi::Monitor(this))
{
    Q_UNUSED(args);
    m_monitor->itemFetchScope().fetchFullPayload();
    m_monitor->itemFetchScope().setFetchModificationTime(false);
#ifdef HAVE_KDEPIM_ATLEAST_412
    m_monitor->itemFetchScope().setFetchRemoteIdentification(false);
#endif
}

AkonadiDataSource::~AkonadiDataSource()
{
}

KPeople::AllContactsMonitor *AkonadiDataSource::createAllContactsMonitor()
{
    return new AkonadiAllContacts();
}

KPeople::ContactMonitor *AkonadiDataSource::createContactMonitor(const QString &contactUri)
{
    return new AkonadiContact(m_monitor, contactUri);
}

K_PLUGIN_FACTORY(AkonadiDataSourceFactory, registerPlugin<AkonadiDataSource>();)
K_EXPORT_PLUGIN(AkonadiDataSourceFactory("akonadi_kpeople_plugin"))

#include "akonadidatasource.moc"
