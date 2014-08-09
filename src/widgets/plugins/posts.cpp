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

#include "posts.h"
#include "plugins/postviewdelegate.h"

#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QStandardItem>
#include <QDebug>
#include <KPluginFactory>
#include <KLocalizedString>

#include <Akonadi/Item>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/Collection>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>
#include <Akonadi/ServerManager>
#include <Akonadi/SocialUtils/SocialFeedItem>

#include <KMime/Message>
#include <KMimeType>
#include <QSizePolicy>
#include <QHeaderView>
#include <KABC/Addressee>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KLocale>
#include <KStandardDirs>
#include <QSqlDatabase>
#include <QSqlQuery>

using namespace Akonadi;


Posts::Posts(QObject *parent): AbstractFieldWidgetFactory(parent)
{
}

QWidget *Posts::createDetailsWidget(const KABC::Addressee &person, const KABC::AddresseeList &contacts, QWidget *parent) const
{
    Q_UNUSED(contacts);
    QWidget *widget = new QWidget(parent);

    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(widget);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFixedHeight(widget->height());

    const_cast<Posts *>(this)->m_person = person;
    const_cast<Posts *>(this)->m_ListView = new QListView();
    const_cast<Posts *>(this)->m_model = new QStandardItemModel();
    const_cast<Posts *>(this)->m_layout = layout;

    m_ListView->setModel(m_model);
    m_ListView->setItemDelegate(new PostViewDelegate());
    m_ListView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if (m_person.custom("akonadi", "id").contains("akonadi://?item=")) {
        QString id = m_person.custom("akonadi", "id").split("akonadi://?item=", QString::SkipEmptyParts).first();
        Item item;
        item.setId(id.toLongLong());
        ItemFetchJob *job = new ItemFetchJob(item);
        connect(job, SIGNAL(finished(KJob *)), this, SLOT(akonadiItemFetchFinished(KJob *)));

    } else if (m_person.custom("akonadi", "id").contains("chat.facebook.com")) {
        QString id = m_person.custom("akonadi", "id");
        id = m_person.custom("akonadi", "id").mid(id.indexOf("chat_2efacebook_2ecom0?-") + 24);
        id = id.left(id.lastIndexOf("@chat.facebook.com"));
        QString DBpath = KGlobal::dirs()->localxdgdatadir() + QLatin1String("akonadi/facebook_post.db");
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(DBpath);
        if (!db.open()) {
            qDebug() << "Database not found";
        }
        QSqlQuery query(db);
        query.prepare("SELECT postId FROM userPost WHERE userId=?");
        query.bindValue(0, id);
        query.exec();
        while (query.next()) {
            const_cast<Posts *>(this)->postsId << query.value(0).toString();
        }
        query.clear();
        db.close();

        if (postsId.length()) {
            CollectionFetchJob *job = new CollectionFetchJob(Collection::root(), CollectionFetchJob::Recursive);
            connect(job, SIGNAL(result(KJob *)), SLOT(collectionResult(KJob *)));
        } else {
            layout->addWidget(new QLabel("Post of Selected contacts are not avaiable"));
        }
    } else {
        layout->addWidget(new QLabel("Post of Selected contacts are not avaiable"));
    }

    layout->addWidget(m_ListView);
    widget->setLayout(layout);
    return scrollArea;
}

void Posts::akonadiItemFetchFinished(KJob *job)
{
    if (job->error()) {
        qDebug() << "Error occurred while fetching item" << job->errorString();
        return;
    }
    Akonadi::ItemFetchJob *fetchJob = qobject_cast<Akonadi::ItemFetchJob *>(job);
    const Akonadi::Item::List items = fetchJob->items();

    if (!items.count())
        return;

    QString DBpath = KGlobal::dirs()->localxdgdatadir() + QLatin1String("akonadi/facebook_posts.db");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(DBpath);
    if (!db.open()) {
        qDebug() << "Database not found";
    }
    QSqlQuery query(db);
    query.prepare("SELECT postId FROM userPost WHERE userId=?");
    query.bindValue(0, items.first().remoteId());
    query.exec();
    while (query.next()) {
        postsId << query.value(0).toString();
    }
    query.clear();
    db.close();

    if (postsId.length()) {
        CollectionFetchJob *cjob = new CollectionFetchJob(Collection::root(), CollectionFetchJob::Recursive);
        connect(cjob, SIGNAL(result(KJob *)), SLOT(collectionResult(KJob *)));
    } else {
        m_layout->addWidget(new QLabel("Post of Selected contacts are not avaiable"));
    }
}

void Posts::collectionResult(KJob *job)
{
    if (job->error()) {
        qDebug() << "Error occurred while Fetching collection" << job->errorString();
        return;
    }
    CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob *>(job);
    const Collection::List collections = fetchJob->collections();
    foreach (const Collection & collection, collections) {
        if (collection.contentMimeTypes().contains(QLatin1String("text/x-vnd.akonadi.socialfeeditem")) && collection.name().contains(QLatin1String("Facebook"))) {
            foreach (QString postid, postsId) {
                Item item;
                item.setRemoteId(postid);
                ItemFetchJob *job = new ItemFetchJob(item);
                job->setCollection(collection);
                job->fetchScope().fetchFullPayload();
                connect(job, SIGNAL(result(KJob *)), SLOT(jobFinished(KJob *)));
            }
        }
    }
}

void Posts::jobFinished(KJob *job)
{
    if (job->error()) {
        qDebug() << "Error occurred on fetching post" << job->errorString();
        return;
    }
    Akonadi::ItemFetchJob *fetchJob = qobject_cast<Akonadi::ItemFetchJob *>(job);
    const Akonadi::Item::List items = fetchJob->items();
    foreach (Item i , items) {
        Akonadi::SocialFeedItem item = i.payload<Akonadi::SocialFeedItem>();
        QStandardItem *post = new QStandardItem();
        post->setData(item.postText(), PostViewDelegate::PostTextRole);
        post->setData(item.postInfo(), PostViewDelegate::PostInfoRole);
        post->setData(KGlobal::locale()->formatDateTime(item.postTime(), KLocale::FancyShortDate), PostViewDelegate::PostTimeRole);
        post->setData(item.postLink(), PostViewDelegate::PostLinkRole);
        m_model->appendRow(post);
    }
}


QString Posts::label() const
{
    return i18n("Posts");
}

int Posts::sortWeight() const
{
    return 0;
}

#include "posts.moc"
