/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  David Edmundson <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "recentemailswidget.h"

#include <QTreeWidget>
#include <QVBoxLayout>
#include <QDateTime>

#include <KLocalizedString>
#include <KIcon>
#include <KToolInvocation>

#include <kpeople/persondata.h>

#include <Nepomuk2/Resource>
#include <Nepomuk2/Query/Query>
#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/ResourceWatcher>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Soprano/Util/AsyncQuery>

using namespace KPeople;

RecentEmailsDetailsWidget::RecentEmailsDetailsWidget(QWidget* parent): AbstractPersonDetailsWidget(parent)
{
    setTitle(i18n("Recent Emails"));
    setIcon(KIcon("mail-message"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    m_tableWidget = new QTreeWidget(this);
    m_tableWidget->setRootIsDecorated(false);
    m_tableWidget->setHeaderHidden(true);
    m_tableWidget->setFrameShape(QFrame::NoFrame);
    connect(m_tableWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(onItemDoubleClicked(QTreeWidgetItem*)));

    layout->addWidget(m_tableWidget);
    setLayout(layout);
}


void RecentEmailsDetailsWidget::setPerson(PersonData* personData)
{
    const QString queryString = QString::fromUtf8("select DISTINCT ?uri ?isRead ?subject ?messageDate ?akonadiUrl WHERE "
                                      "{"
                                      "?uri a nmo:Email."
                                      "{<%1> a nco:PersonContact." //if PersonData URI represents a contact
                                      "?uri nmo:from <%1>.}" //and email is from this contact
                                      "UNION" //OR
                                      "{<%1> a pimo:Person." //if the URI is a person, and email is from one of the sub contacts
                                      "<%1> pimo:groundingOccurrence ?contactUri."
                                      "?uri nmo:from ?contactUri.}"
                                      "?uri nmo:isRead ?isRead."
                                      "?uri nmo:messageSubject ?subject."
                                      "?uri nmo:sentDate ?messageDate."
                                      "?uri nie:url ?akonadiUrl"
                                      "}"
                                      "LIMIT 20"
                                     ).arg(personData->uri());

    Soprano::Model *model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::Util::AsyncQuery *query = Soprano::Util::AsyncQuery::executeQuery(model, queryString, Soprano::Query::QueryLanguageSparql);

    connect(query, SIGNAL(finished(Soprano::Util::AsyncQuery*)),
            SLOT(onLoadFinished(Soprano::Util::AsyncQuery*)));
    connect(query, SIGNAL(nextReady(Soprano::Util::AsyncQuery*)),
            SLOT(onNextReady(Soprano::Util::AsyncQuery*)));

    setActive(false);
    m_tableWidget->clear();
}

void RecentEmailsDetailsWidget::onItemDoubleClicked(QTreeWidgetItem* item)
{
    const QString &akonadiUrl = item->data(0, Qt::UserRole).toString();

    if (!akonadiUrl.isEmpty()) {
        KToolInvocation::kdeinitExec("kmail", QStringList() << "--view" << akonadiUrl);
    }
}

void RecentEmailsDetailsWidget::onNextReady(Soprano::Util::AsyncQuery* query)
{
    const QString &uri = query->binding("uri").uri().toString();
    bool isRead = query->binding("isRead").literal().toBool();
    const QString &subject = query->binding("subject").toString();
    const QDateTime &time = query->binding("messageDate").literal().toDateTime();
    const QString &akonadiUrl = query->binding("akonadiUrl").toString();

    QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << subject << time.toString());
    item->setData(0, Qt::UserRole, akonadiUrl);
    item->setIcon(0, isRead ? KIcon("mail-message-read") : KIcon("mail-message-unread"));
    m_tableWidget->addTopLevelItem(item);
    query->next();
}

void RecentEmailsDetailsWidget::onLoadFinished(Soprano::Util::AsyncQuery* query)
{
    setActive(m_tableWidget->model()->rowCount() > 0);
}
