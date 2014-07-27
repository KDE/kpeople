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

#include <QVBoxLayout>
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

#include <KMime/Message>
#include <KMimeType>
#include <QSizePolicy>
#include <QHeaderView>
#include <KABC/Addressee>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KLocale>
#include <akonadi/socialutils/socialfeeditem.h>

#include <QDebug>

using namespace Akonadi;


Posts::Posts(QObject* parent): AbstractFieldWidgetFactory(parent)
{
}

QWidget* Posts::createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList& contacts, QWidget* parent) const
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

    const_cast<Posts*>(this)->m_person = person;
    const_cast<Posts*>(this)->m_ListView = new QListView();
    const_cast<Posts*>(this)->m_model = new QStandardItemModel();

    m_ListView->setModel(m_model);
    m_ListView->setItemDelegate(new PostViewDelegate());
    m_ListView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ListView->show();

    CollectionFetchJob* job = new CollectionFetchJob(Collection::root(), CollectionFetchJob::Recursive);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(collectionResult(KJob*)));

    layout->addWidget(m_ListView);
    widget->setLayout(layout);
    return scrollArea;
}

void Posts::collectionResult(KJob* job)
{
    if (job->error()) {
        qDebug() << "Error occurred";
        return;
    }
    CollectionFetchJob* fetchJob = qobject_cast<CollectionFetchJob*>(job);
    const Collection::List collections = fetchJob->collections();
    foreach (const Collection & collection, collections) {
        if (collection.contentMimeTypes().contains(QLatin1String("text/x-vnd.akonadi.socialfeeditem")) && collection.name().contains(QLatin1String("Facebook"))) {
            Akonadi::ItemFetchJob* job = new Akonadi::ItemFetchJob(collection);
            connect(job, SIGNAL(result(KJob*)), SLOT(jobFinished(KJob*)));
            job->fetchScope().fetchFullPayload();
        }
    }
}

void Posts::jobFinished(KJob* job)
{
    if (job->error()) {
        qDebug() << "Error occurred";
        return;
    }
    Akonadi::ItemFetchJob* fetchJob = qobject_cast<Akonadi::ItemFetchJob*>(job);
    const Akonadi::Item::List items = fetchJob->items();
    foreach (Item i , items) {
        Akonadi::SocialFeedItem item = i.payload<Akonadi::SocialFeedItem>();
        if (m_person.uid().compare(item.userId()) == 0) {
            QStandardItem* post = new QStandardItem();
            post->setData(item.postText(), PostViewDelegate::PostTextRole);
            post->setData(item.postInfo(), PostViewDelegate::PostInfoRole);
            post->setData(KGlobal::locale()->formatDateTime(item.postTime(), KLocale::FancyShortDate), PostViewDelegate::PostTimeRole);
            post->setData(item.postLink(), PostViewDelegate::PostLinkRole);
            m_model->appendRow(post);
        }
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
