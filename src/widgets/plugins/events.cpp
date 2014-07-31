/*
 * Copyright 2014  Nilesh Suthar <nileshsuthar@live.in>
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

#include "events.h"
#include "plugins/eventviewdelegate.h"
#include <QLabel>
#include <KLocalizedString>
#include <QScrollArea>
#include <QStandardItem>
#include <QDebug>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <libkfbapi/alleventslistjob.h>
#include <libkfbapi/eventjob.h>
#include <KCalCore/Attendee>
#include <KLocale>
#include <KStandardDirs>
#include <QSqlDatabase>
#include <QSqlQuery>

using namespace Akonadi;

Events::Events(QObject* parent): AbstractFieldWidgetFactory(parent)
{
}
QWidget* Events::createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList& contacts, QWidget* parent) const
{
    Q_UNUSED(contacts);
    QWidget* widget = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(widget);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFixedHeight(widget->height());

    const_cast<Events*>(this)->m_person = person;
    const_cast<Events*>(this)->m_layout = layout;
    const_cast<Events*>(this)->m_listView = new QListView();
    const_cast<Events*>(this)->m_model = new QStandardItemModel();

    m_listView->setItemDelegate(new EventViewDelegate());
    m_listView->setModel(m_model);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if (m_person.custom("akonadi", "id").contains("akonadi://?item=")) {
        QString id = m_person.custom("akonadi", "id").split("akonadi://?item=", QString::SkipEmptyParts).first();
        Item item;
        item.setId(id.toLongLong());
        ItemFetchJob* job = new ItemFetchJob(item);
        connect(job, SIGNAL(finished(KJob*)), this, SLOT(akonadiItemFetchFinished(KJob*)));
    } else if (m_person.custom("akonadi", "id").contains("chat.facebook.com")) {
        QString id = m_person.custom("akonadi", "id");
        id = m_person.custom("akonadi", "id").mid(id.indexOf("chat_2efacebook_2ecom0?-") + 24);
        id = id.left(id.lastIndexOf("@chat.facebook.com"));
        QString DBpath = KGlobal::dirs()->localxdgdatadir() + QLatin1String("akonadi/facebook_events.db");
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(DBpath);
        if (!db.open()) {
            qDebug() << "Database not found";
        }
        QSqlQuery query;
        query.prepare("SELECT eventId FROM userEvent WHERE userId=?");
        query.bindValue(0, id);
        query.exec();
        while (query.next()) {
            const_cast<Events*>(this)->eventId << query.value(0).toLongLong();
        }
        db.close();
        if (eventId.length()) {
            CollectionFetchJob* job = new CollectionFetchJob(Collection::root(), CollectionFetchJob::Recursive);
            connect(job, SIGNAL(result(KJob*)), SLOT(fetchFinished(KJob*)));
        } else {
            layout->addWidget(new QLabel("Events of Selected contacts are not avaiable"));
        }
    } else {
        layout->addWidget(new QLabel("Events of Selected contacts are not avaiable"));
    }
    layout->addWidget(m_listView);
    widget->setLayout(layout);
    return scrollArea;
}
void Events::akonadiItemFetchFinished(KJob* job)
{
    if (job->error()) {
        qDebug() << "Error occurred" << job->errorString();
        return;
    }
    Akonadi::ItemFetchJob* fetchJob = qobject_cast<Akonadi::ItemFetchJob*>(job);
    const Akonadi::Item::List items = fetchJob->items();

    if (!items.count())
        return;

    QString DBpath = KGlobal::dirs()->localxdgdatadir() + QLatin1String("akonadi/facebook_events.db");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DBpath);
    if (!db.open()) {
        qDebug() << "Database not found";
    }
    QSqlQuery query;
    query.prepare("SELECT eventId FROM userEvent WHERE userId=?");
    query.bindValue(0, items.first().remoteId());
    query.exec();
    while (query.next()) {
        eventId << query.value(0).toLongLong();
    }
    db.close();
    if (eventId.length()) {
        CollectionFetchJob* cjob = new CollectionFetchJob(Collection::root(), CollectionFetchJob::Recursive);
        connect(cjob, SIGNAL(result(KJob*)), SLOT(fetchFinished(KJob*)));
    } else {
        m_layout->addWidget(new QLabel("Events of Selected contacts are not avaiable"));
    }
}

void Events::fetchFinished(KJob* job)
{
    if (job->error()) {
        qDebug() << "Error occurred" << job->errorString();
        return;
    }
    CollectionFetchJob* fetchJob = qobject_cast<CollectionFetchJob*>(job);
    const Collection::List collections = fetchJob->collections();
    foreach (const Collection & collection, collections) {
        if (collection.contentMimeTypes().contains(QLatin1String("application/x-vnd.akonadi.calendar.event")) && collection.name().contains(QLatin1String("Facebook"))) {
            foreach (qlonglong eventid, eventId) {
                Item item;
                item.setRemoteId(QString::number(eventid));
                ItemFetchJob* job = new ItemFetchJob(item);
                job->setCollection(collection);
                job->fetchScope().fetchFullPayload();
                connect(job, SIGNAL(result(KJob*)), SLOT(jobFinished(KJob*)));
            }
        }
    }
}

void Events::jobFinished(KJob* job)
{
    if (job->error()) {
        qDebug() << "Error occurred";
        return;
    }
    Akonadi::ItemFetchJob* fetchJob = qobject_cast<Akonadi::ItemFetchJob*>(job);
    const Akonadi::Item::List items = fetchJob->items();
    foreach (const Akonadi::Item & item, items) {
        KFbAPI::IncidencePtr i = item.payload<KFbAPI::IncidencePtr>();
        QStandardItem* event = new QStandardItem();
        event->setData(i.data()->summary(), EventViewDelegate::EventNameRole);
//               event->setData(i.data()->description(),EventViewDelegate::EventDescRole);
        KDateTime startDate = i.data()->dateTime(KCalCore::IncidenceBase::RoleDisplayStart);
        KDateTime endDate = i.data()->dateTime(KCalCore::IncidenceBase::RoleDisplayEnd);
        event->setData(KGlobal::locale()->formatDateTime(startDate, KLocale::FancyShortDate), EventViewDelegate::EventDateRole);
        event->setData(KGlobal::locale()->formatDuration(startDate.secsTo(endDate))
                       , EventViewDelegate::EventDurationRole);
        event->setData(i.data()->location(), EventViewDelegate::EventLocationRole);
        event->setData(i.data()->url().toString(), EventViewDelegate::EventLinkRole);
        m_model->appendRow(event);
//                 m_layout->addWidget(new QLabel(i.data()->summary()));
    }
}


QString Events::label() const
{
    return i18n("Events");
}
int Events::sortWeight() const
{
    return 0;
}

#include "events.moc"
