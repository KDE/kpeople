/*
    Copyright (C) 2013  David Edmundson (davidedmundson@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "persondata.h"

#include "metacontact_p.h"
#include "personmanager_p.h"
#include "personpluginmanager_p.h"
#include "backends/basepersonsdatasource.h"
#include "backends/contactmonitor.h"
#include "backends/abstractcontact.h"

#include <QUrl>
#include <QDebug>
#include <QStandardPaths>

namespace KPeople
{
class PersonDataPrivate
{
public:
    QStringList contactIds;
    MetaContact metaContact;
    QList<ContactMonitorPtr> watchers;
};
}

using namespace KPeople;

KPeople::PersonData::PersonData(const QString &id, QObject *parent):
    QObject(parent),
    d_ptr(new PersonDataPrivate)
{
    Q_D(PersonData);

    QString personId;
    //query DB
    if (id.startsWith(QLatin1String("kpeople://"))) {
        personId = id;
    } else {
        personId = PersonManager::instance()->personIdForContact(id);
    }

    if (personId.isEmpty()) {
        d->contactIds = QStringList() << id;
    } else {
        d->contactIds = PersonManager::instance()->contactsForPersonId(personId);
    }

    QMap<QString, AbstractContact::Ptr> contacts;
    Q_FOREACH (const QString &contactId, d->contactIds) {
        //load the correct data source for this contact ID
        const QString sourceId = contactId.left(contactId.indexOf(QStringLiteral("://")));
        BasePersonsDataSource *dataSource = PersonPluginManager::dataSource(sourceId);
        if (dataSource) {
            ContactMonitorPtr cw = dataSource->contactMonitor(contactId);
            d->watchers << cw;

            //if the data source already has the contact set it already
            //if not it will be loaded when the contactChanged signal is emitted
            if (cw->contact()) {
                contacts[contactId] = cw->contact();
            }
            connect(cw.data(), SIGNAL(contactChanged()), SLOT(onContactChanged()));
        } else
            qWarning() << "error: creating PersonData for unknown contact" << contactId;
    }

    d->metaContact = MetaContact(personId, contacts);
}

PersonData::~PersonData()
{
    delete d_ptr;
}

QString PersonData::personId() const
{
    Q_D(const PersonData);
    return d->metaContact.id();
}

QStringList PersonData::contactIds() const
{
    Q_D(const PersonData);
    return d->metaContact.contactIds();
}

void PersonData::onContactChanged()
{
    Q_D(PersonData);

    ContactMonitor *watcher = qobject_cast<ContactMonitor *>(sender());
    if (d->metaContact.contactIds().contains(watcher->contactId())) {
#warning probably not needed anymore
        d->metaContact.updateContact(watcher->contactId(), watcher->contact());
    } else {
        d->metaContact.insertContact(watcher->contactId(), watcher->contact());
    }
    Q_EMIT dataChanged();
}

QPixmap PersonData::photo() const
{
    Q_D(const PersonData);
    QPixmap avatar;

    QVariant pic = contactCustomProperty(AbstractContact::PictureProperty);
    if (pic.canConvert<QImage>()) {
        avatar = QPixmap::fromImage(pic.value<QImage>());
    } else if (pic.canConvert<QUrl>()) {
        avatar = QPixmap(pic.toUrl().toLocalFile());
    }

    if (avatar.isNull()) {
        static QString defaultAvatar = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kpeople/dummy_avatar.png"));
        avatar = QPixmap(defaultAvatar);
    }
    return avatar;
}
QVariant PersonData::contactCustomProperty(const QString &key) const
{
    Q_D(const PersonData);
    return d->metaContact.personAddressee()->customProperty(key);
}

QString PersonData::presenceIconName() const
{
    Q_D(const PersonData);
    QString contactPresence = contactCustomProperty(QStringLiteral("telepathy-presence")).toString();
    return KPeople::iconNameForPresenceString(contactPresence);
}

QString PersonData::name() const
{
    return contactCustomProperty(AbstractContact::NameProperty).toString();
}

QString PersonData::presence() const
{
    return contactCustomProperty(AbstractContact::PresenceProperty).toString();
}

QUrl PersonData::pictureUrl() const
{
    return contactCustomProperty(AbstractContact::PictureProperty).toUrl();
}

QString PersonData::email() const
{
    return contactCustomProperty(AbstractContact::EmailProperty).toString();
}

QStringList PersonData::groups() const
{
//     We might want to cache it eventually?

    QVariantList groups = contactCustomProperty(AbstractContact::GroupsProperty).toList();
    QStringList ret;
    Q_FOREACH (const QVariant &g, groups) {
        Q_ASSERT(g.canConvert<QString>());
        ret += g.toString();
    }
    ret.removeDuplicates();
    return ret;
}

QStringList PersonData::allEmails() const
{
    QVariantList emails = contactCustomProperty(AbstractContact::AllEmailsProperty).toList();
    QStringList ret;
    Q_FOREACH (const QVariant &e, emails) {
        Q_ASSERT(e.canConvert<QString>());
        ret += e.toString();
    }
    ret.removeDuplicates();
    return ret;
}

