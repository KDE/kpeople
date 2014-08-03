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

#include "chat.h"
#include "chatlistviewdelegate.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDebug>
#include <KLocalizedString>
#include <KGlobal>
#include <KABC/Addressee>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KLocale>
#include <complex>
#include <KTp/core.h>
#include <KTp/Logger/log-manager.h>
#include <KTp/Logger/log-entity.h>
#include <KTp/Logger/pending-logger-logs.h>
#include <KTp/message.h>
#include <TelepathyQt/Account>
#include <TelepathyQt/AccountManager>


Chat::Chat(QObject *parent): AbstractFieldWidgetFactory(parent)
{
}

QWidget *Chat::createDetailsWidget(const KABC::Addressee &person, const KABC::AddresseeList &contacts, QWidget *parent) const
{
    Q_UNUSED(contacts);
    QWidget *widget = new QWidget(parent);

    const_cast<Chat *>(this)->m_chatwidget = widget;

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(widget);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFixedHeight(widget->height());

    QVBoxLayout *layout = new QVBoxLayout(widget);
    QListView *lv = new QListView();
    ChatListviewDelegate *ch = new ChatListviewDelegate();

    const_cast<Chat *>(this)->m_chatlistView = lv;
    const_cast<Chat *>(this)->model = new QStandardItemModel();



    lv->setModel(model);

    lv->setItemDelegate(ch);
    layout->setContentsMargins(0, 0, 0, 0);
    if (person.custom("telepathy", "accountPath").isEmpty()) {
        layout->addWidget(new QLabel("Chats for current contact is not supported"));
    } else {
        KTp::LogManager *logManager = KTp::LogManager::instance();
        logManager->setAccountManager(KTp::accountManager());
        KTp::LogEntity logEntity = KTp::LogEntity(Tp::HandleTypeContact, person.custom("telepathy", "contactId"));

        //FIXME Use some proper method to get account using accountPath
        Tp::AccountPtr account = KTp::accountManager().data()->accountForObjectPath("/org/freedesktop/Telepathy/Account/" + person.custom("telepathy", "accountPath"));

        if (account.isNull()) {
            qDebug() << "Error Occoured Account is not supposed to be null";
        } else {
            if (logManager->logsExist(account, logEntity)) {
                KTp::PendingLoggerDates *pd = logManager->queryDates(account, logEntity);
                if (!pd) {
                    qWarning() << "Error in PendingDates";
                } else {
                    connect(pd, SIGNAL(finished(KTp::PendingLoggerOperation *)), SLOT(onPendingDates(KTp::PendingLoggerOperation *)));
                }
            } else {
                layout->addWidget(new QLabel("Chats for current contact is not available"));
            }
        }
    }

    layout->addWidget(lv);
    widget->setLayout(layout);

    return scrollArea;
}
void Chat::onPendingDates(KTp::PendingLoggerOperation *po)
{

    KTp::PendingLoggerDates *pd = qobject_cast<KTp::PendingLoggerDates *>(po);
    QList<QDate> dates = pd->dates();
    if (dates.isEmpty()) {
        qDebug() << "No messages";
    }
    KTp::PendingLoggerLogs *log = KTp::LogManager::instance()->queryLogs(pd->account(), pd->entity(), dates.last());
    connect(log, SIGNAL(finished(KTp::PendingLoggerOperation *)), this, SLOT(onEventsFinished(KTp::PendingLoggerOperation *)));
}

void Chat::onEventsFinished(KTp::PendingLoggerOperation *op)
{
    KTp::PendingLoggerLogs *logsOp = qobject_cast<KTp::PendingLoggerLogs *>(op);
    if (logsOp->hasError()) {
        kWarning() << "Failed to fetch events:" << logsOp->error();
        return;
    }
    QStringList queuedMessageTokens;
    QList<KTp::LogMessage> ml = logsOp->logs();
    QString date = KGlobal::locale()->formatDate(logsOp->date(), KLocale::FancyShortDate);
//     m_chatwidget->layout()->addWidget(new QLabel("Chat histrory from " + date));
    foreach (KTp::LogMessage message, ml) {

        if (message.direction()) {
            QStandardItem *messageRow = new QStandardItem();

            messageRow->setData(message.senderAlias(), ChatListviewDelegate::senderAliasRole);
            messageRow->setData(message.mainMessagePart(), ChatListviewDelegate::messageRole);
            messageRow->setData(KGlobal::locale()->formatTime(message.time().time()), ChatListviewDelegate::messageTimeRole);

            model->appendRow(messageRow);
        } else {

            QStandardItem *messageRow = new QStandardItem();

            messageRow->setData("Me", ChatListviewDelegate::senderAliasRole);
            messageRow->setData(message.mainMessagePart(), ChatListviewDelegate::messageRole);
            messageRow->setData(KGlobal::locale()->formatTime(message.time().time()), ChatListviewDelegate::messageTimeRole);
            model->appendRow(messageRow);
        }
    }
}


QString Chat::label() const
{
    return i18n("Chat");
}

int Chat::sortWeight() const
{
    return 0;
}

#include "chat.moc"
