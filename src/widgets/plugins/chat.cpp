/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2014  <copyright holder> <email>
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
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <KLocalizedString>
#include <KABC/Addressee>
#include <KPluginFactory>
#include <KPluginLoader>
#include <QDir>
#include <KStandardDirs>
#include <KTp/Logger/log-manager.h>
#include <KTp/Logger/log-entity.h>
#include <TelepathyQt/Account>
#include <KPeople/PersonsModel>

Chat::Chat(QObject* parent): AbstractFieldWidgetFactory(parent)
{
}

QWidget* Chat::createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList &contacts, QWidget* parent) const
{
    Q_UNUSED(contacts);
    QWidget *widget = new QWidget(parent);

    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0,0,0,0);
    if(person.custom("telepathy","accountPath").isEmpty()) {
        layout->addWidget(new QLabel("Chats for current contact is not available"));
    } else {

        KTp::LogManager *logManager = KTp::LogManager::instance();
        layout->addWidget(new QLabel("Work in Progress"));
        qDebug() << person.custom("akonadi","id");
        KTp::LogEntity logEntity = KTp::LogEntity(Tp::HandleTypeContact,person.custom("telepathy","contactId"));
        Tp::AccountPtr account;

//       Tp::AccountPtr account(index.data(KTp::AccountsListModel::AccountRole).value<Tp::AccountPtr>());


        qDebug() << logManager->logsExist(account,logEntity);
        qDebug() << logEntity.isValid();
//       qDebug() << logEntity.entityType();
//       qDebug() << logEntity.id();

//       foreach(QString s , person.customs()){
// 	 qDebug() << s;
//
//       }

    }

    widget->setLayout(layout);
    return widget;
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
