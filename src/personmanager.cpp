/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  David Edmundson <davidedmundson@kde.org>
 * Copyright 2013  Martin Klapetek <mklapetek@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "personmanager_p.h"

// #include "personmanager.moc"
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QDBusConnection>
#include <QDBusMessage>
#include <KStandardDirs>

class Transaction
{
public:
    Transaction(const QSqlDatabase &db);
    void cancel();
    ~Transaction();
private:
    QSqlDatabase m_db;
    bool m_cancelled;
};

Transaction::Transaction(const QSqlDatabase& db) :
    m_db(db),
    m_cancelled(false)
{
    m_db.transaction();
}

void Transaction::cancel()
{
    m_db.rollback();
    m_cancelled = true;
}

Transaction::~Transaction()
{
    if (!m_cancelled) {
        m_db.commit();
    }
}

PersonManager::PersonManager(const QString &databasePath, QObject *parent):
    QObject(parent),
    m_db(QSqlDatabase::addDatabase("QSQLITE"))
{
    m_db.setDatabaseName(databasePath);
    m_db.open();
    m_db.exec("CREATE TABLE IF NOT EXISTS persons (contactID VARCHAR UNIQUE NOT NULL, personID INT NOT NULL)");
    m_db.exec("CREATE INDEX IF NOT EXISTS contactIdIndex ON persons (contactId)");
    m_db.exec("CREATE INDEX IF NOT EXISTS personIdIndex ON persons (personId)");

    QDBusConnection::sessionBus().connect(QString(), QString("/KPeople"), "org.kde.KPeople", "ContactAddedToPerson", this, SIGNAL(contactAddedToPerson(const QString&, const QString&)));
    QDBusConnection::sessionBus().connect(QString(), QString("/KPeople"), "org.kde.KPeople", "ContactRemovedFromPerson", this, SIGNAL(contactRemovedFromPerson(const QString&)));
}

PersonManager::~PersonManager()
{

}

QMultiHash< QString, QString > PersonManager::allPersons() const
{
    QMultiHash<QString /*PersonID*/, QString /*ContactID*/> contactMapping;

    QSqlQuery query = m_db.exec("SELECT personID, contactID FROM persons");
    while (query.next()) {
        const QString personId = "kpeople://" + query.value(0).toString(); // we store as ints internally, convert it to a string here
        const QString contactID = query.value(1).toString();
        contactMapping.insertMulti(personId, contactID);
    }
    return contactMapping;
}

QStringList PersonManager::contactsForPersonId(const QString& personId) const
{
    if (!personId.startsWith("kpeople://")) {
        return QStringList();
    }

    QStringList contactIds;
    //TODO port to the proper qsql method for args
    QSqlQuery query(m_db);
    query.prepare("SELECT contactID FROM persons WHERE personId = ?");
    query.bindValue(0, personId.mid(strlen("kpeople://")));
    query.exec();

    while (query.next()) {
        contactIds << query.value(0).toString();
    }
    return contactIds;
}

QString PersonManager::personIdForContact(const QString& contactId) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT personId FROM persons WHERE contactId = ?");
    query.bindValue(0, contactId);
    query.exec();
    if (query.next()) {
        return "kpeople://"+query.value(0).toString();
    }
    return QString();
}


QString PersonManager::mergeContacts(const QStringList& ids)
{
    // no merging if we have only 0 || 1 ids
    if (ids.size() < 2) {
        return QString();
    }

    QStringList metacontacts;
    QStringList contacts;

    bool rc = true;

    QList<QDBusMessage> pendingMessages;

    // separate the passed ids to metacontacts and simple contacts
    Q_FOREACH (const QString &id, ids) {
        if (id.startsWith(QLatin1String("kpeople://"))) {
            metacontacts << id;
        } else {
            contacts << id;
        }
    }

    // create new personIdString
    //   - if we're merging two simple contacts, create completely new id
    //   - if we're merging an existing metacontact, take the first id and use it
    QString personIdString;
    if (metacontacts.count() == 0) {
        // query for the highest existing ID in the database and +1 it
        int personId = 0;
        QSqlQuery query = m_db.exec(QString("SELECT MAX(personID) FROM persons"));
        if (query.next()) {
            personId = query.value(0).toInt();
            personId++;
        }

        personIdString = "kpeople://" + QString::number(personId);
    } else {
        personIdString = metacontacts.first();
    }

    // start a db transaction (ends automatically on destruction)
    Transaction t(m_db);

    // processed passed metacontacts
    if (metacontacts.count() > 1) {
        // collect all the contacts from other persons
        QStringList personContacts;
        Q_FOREACH (const QString &id, metacontacts) {
            if (id != personIdString) {
                personContacts << contactsForPersonId(id);
            }
        }

        // iterate over all of the contacts and change their personID to the new personIdString
        Q_FOREACH (const QString &id, personContacts) {
            QSqlQuery updateQuery(m_db);
            updateQuery.prepare("UPDATE persons SET personID = ? WHERE contactID = ?");
            updateQuery.bindValue(0, personIdString.mid(strlen("kpeople://")));
            updateQuery.bindValue(1, id);
            if (!updateQuery.exec()) {
                rc = false;
            }

            QDBusMessage message = QDBusMessage::createSignal(QLatin1String("/KPeople"),
                                                              QLatin1String("org.kde.KPeople"),
                                                              QLatin1String("ContactRemovedFromPerson"));

            message.setArguments(QVariantList() << id);
            pendingMessages << message;

            message = QDBusMessage::createSignal(QLatin1String("/KPeople"),
                                                              QLatin1String("org.kde.KPeople"),
                                                              QLatin1String("ContactAddedToPerson"));

            message.setArguments(QVariantList() << id << personIdString);

        }
    }

    // process passed contacts
    if (contacts.size() > 0) {


        Q_FOREACH (const QString &id, contacts) {
            QSqlQuery insertQuery(m_db);
            insertQuery.prepare("INSERT INTO persons VALUES (?, ?)");
            insertQuery.bindValue(0, id);
            insertQuery.bindValue(1, personIdString.mid(strlen("kpeople://"))); //strip kpeople://
            if (!insertQuery.exec()) {
                rc = false;
            }

            //FUTURE OPTIMIZATION - this would be best as one signal, but arguments become complex
            QDBusMessage message = QDBusMessage::createSignal(QLatin1String("/KPeople"),
                                                        QLatin1String("org.kde.KPeople"),
                                                        QLatin1String("ContactAddedToPerson"));

            message.setArguments(QVariantList() << id << personIdString);
            pendingMessages << message;
        }
    }

    //if success send all messages to other clients
    //otherwise roll back our database changes and return an empty string
    if (rc) {
        Q_FOREACH(const QDBusMessage &message, pendingMessages) {
            QDBusConnection::sessionBus().send(message);
        }
    } else {
        t.cancel();
        personIdString.clear();
    }

    return personIdString;
}

bool PersonManager::unmergeContact(const QString &id)
{
    //remove rows from DB
    if (id.startsWith("kpeople://")) {
        QSqlQuery query(m_db);

        const QStringList contactIds = contactsForPersonId(id);
        query.prepare("DELETE FROM persons WHERE personId = ?");
        query.bindValue(0, id.mid(strlen("kpeople://")));
        query.exec();

        Q_FOREACH(const QString &contactId, contactIds) {
            //FUTURE OPTIMIZATION - this would be best as one signal, but arguments become complex
            QDBusMessage message = QDBusMessage::createSignal(QLatin1String("/KPeople"),
                                                      QLatin1String("org.kde.KPeople"),
                                                      QLatin1String("ContactRemovedFromPerson"));

            message.setArguments(QVariantList() << contactId);
            QDBusConnection::sessionBus().send(message);
        }
    } else {
        QSqlQuery query(m_db);
        query.prepare("DELETE FROM persons WHERE contactId = ?");
        query.bindValue(0, id);
        query.exec();
        //emit signal(dbus)
        Q_EMIT contactRemovedFromPerson(id);
    }

    //TODO return if removing rows worked
    return true;
}

PersonManager* PersonManager::instance(const QString &databasePath)
{
    static PersonManager* s_instance = 0;
    if (!s_instance) {
        QString path = databasePath;
        if (path.isEmpty()) {
            path = KGlobal::dirs()->locateLocal("data","kpeople/persondb");
        }
        s_instance = new PersonManager(path);
    }
    return s_instance;
}
