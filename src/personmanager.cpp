/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  David Edmundson <davidedmundson@kde.org>
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

#include "personmanager.h"

// #include "personmanager.moc"
#include <QVariant>
#include <QDebug>
#include <QSqlError>
#include <QDBusConnection>
#include <QDBusMessage>

PersonManager::PersonManager(QObject* parent):
    QObject(parent),
    m_db(QSqlDatabase::addDatabase("QSQLITE3"))
{
    m_db.setDatabaseName("/home/david/persondb");
    m_db.open();
    m_db.exec("CREATE TABLE IF NOT EXISTS PERSONS (contactID VARCHAR UNIQUE NOT NULL, personID INT NOT NULL)");
    //TODO ADD INDEX ON BOTH COLUMNS

    QDBusConnection::sessionBus().connect(QString(), QString("/KPeople"), "org.kde.KPeople", "ContactAddedToPerson", this, SIGNAL(contactAddedToPerson(const QString&, const QString&)));
    QDBusConnection::sessionBus().connect(QString(), QString("/KPeople"), "org.kde.KPeople", "contactRemovedFromPerson", this, SIGNAL(contactRemovedFromPerson(const QString&, const QString&)));
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
    return contactId;
}


QString PersonManager::mergeContacts(const QStringList& ids)
{
    //TODO, ask mck182 about the logic here
    //for now assume all input is unmerged contacts

    int personId = 0;

    //find personID to use
    QSqlQuery query = m_db.exec(QString("SELECT MAX(personId) FROM persons"));
    if (query.next()) {
        personId = query.value(0).toInt();
    }

    personId++;
    QString personIdString = "kpeople://" + QString::number(personId); //FIXME

    //add contacts to insert
    Q_FOREACH(const QString &contactId, ids) {
        if (contactId.startsWith("kpeople://")) {
            qDebug() << "WARNING: Trying to merge a person into a person. Client is sending bad data.";
            continue;
        }

        QSqlQuery insertQuery(m_db);
        insertQuery.prepare("INSERT INTO persons VALUES (?, ?)");
        insertQuery.bindValue(0, contactId);
        insertQuery.bindValue(1, personId);
        insertQuery.exec();

        //FUTURE OPTIMISATION - this would be best as one signal, but arguments become complex
        QDBusMessage message = QDBusMessage::createSignal(QLatin1String("/KPeople"),
                                                    QLatin1String("org.kde.KPeople"),
                                                    QLatin1String("ContactAddedToPerson"));

        message.setArguments(QVariantList() << contactId << personIdString);
        QDBusConnection::sessionBus().send(message);
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
            //FUTURE OPTIMISATION - this would be best as one signal, but arguments become complex
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

PersonManager* PersonManager::instance()
{
    static PersonManager* s_instance = 0;
    if (!s_instance) {
        s_instance = new PersonManager();
    }
    return s_instance;
}
