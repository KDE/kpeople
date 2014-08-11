/*
    Copyright 2014  Nilesh Suthar <nileshsuthar@live.in>

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

#include "emails.h"
#include "plugins/emaillistmodel.h"
#include "plugins/emaillistviewdelegate.h"

#include <QDebug>
#include <QLabel>
#include <QListView>
#include <QVBoxLayout>

#include <KMimeType>
#include <KMime/Message>
#include <QDesktopServices>
#include <KABC/Addressee>
#include <KLocalizedString>

#include <baloo/query.h>
#include <baloo/resultiterator.h>

#include <Akonadi/Item>
#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>

using namespace Akonadi;

Emails::Emails(QObject *parent): AbstractFieldWidgetFactory(parent)
{
}

QWidget *Emails::createDetailsWidget(const KABC::Addressee &person, const KABC::AddresseeList &contacts, QWidget *parent) const
{
    Q_UNUSED(contacts);
    QWidget *widget = new QWidget(parent);

    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    //Fetches emails from Baloo using person preferredEmail
    Baloo::Query query;
    query.setSearchString(person.preferredEmail());
    query.setType("Email");
    Baloo::ResultIterator rt = query.exec();
    QList<Akonadi::Item > itemList;

    while (rt.next()) {
        //Complete msg is not return so fetches the complete mail from akonadi
        itemList << Item::fromUrl(rt.url());
    }

    if (!itemList.empty()) {
        ItemFetchJob *itemFetchJob = new ItemFetchJob(itemList);
        itemFetchJob->fetchScope().fetchFullPayload();
        connect(itemFetchJob, SIGNAL(finished(KJob*)), SLOT(jobFinished(KJob*)));

        QListView *listview = new QListView(widget);
        const_cast<Emails *>(this)->m_emailsModel = new EmailListModel(listview);
        listview->setModel(m_emailsModel);
        listview->setItemDelegate(new EmailListViewDelegate());
        listview->setSelectionMode(QAbstractItemView::SingleSelection);
        listview->setSelectionBehavior(QAbstractItemView::SelectRows);
        listview->setEditTriggers(QAbstractItemView::NoEditTriggers);
        layout->addWidget(listview);

        connect(listview, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onEmailDoubleClicked(QModelIndex)));
    } else {
        layout->addWidget(new QLabel(i18n("No Emails")));
    }
    widget->setLayout(layout);
    return widget;
}

void Emails::onEmailDoubleClicked(const QModelIndex &clicked)
{
    QUrl url = m_emailsModel->getItemUrl(clicked.row());
    QDesktopServices::openUrl(url);
}

void Emails::jobFinished(KJob *job)
{
    if (job->error()) {
        qDebug() << "Error:" << job->errorString();
        return;
    }

    Akonadi::ItemFetchJob *fetchJob = qobject_cast<Akonadi::ItemFetchJob *>(job);
    const Akonadi::Item::List items = fetchJob->items();
    KMime::Message msg;

    foreach (const Akonadi::Item & item, items) {
        msg.setContent(item.payloadData());
        msg.setFrozen(true);
        msg.parse();
        KMime::Headers::Subject *subject = msg.subject();
        KMime::Headers::Date *date = msg.date();
        KMime::Content *textContent = msg.textContent();

        EmailItem mail;
        mail.subject = subject->asUnicodeString();
        mail.date = date->dateTime();
        mail.body = textContent->body();
        mail.url = item.url();

        m_emailsModel->addEmail(mail);
    }
}

QString Emails::label() const
{
    return i18n("Mails");
}

int Emails::sortWeight() const
{
    return 0;
}

#include "emails.moc"
