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

Chat::Chat(QObject* parent): AbstractFieldWidgetFactory(parent)
{
}

QWidget* Chat::createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList &contacts, QWidget* parent) const
{
    Q_UNUSED(contacts);
    QWidget *widget = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0,0,0,0);
    KStandardDirs dirs;

    QString path = dirs.localxdgdatadir() + QDir::separator() + QLatin1String("TpLogger") + QDir::separator() + QLatin1String("logs");

    QString id = person.custom("akonadi","id");
    if (id.startsWith(QLatin1String("ktp://"))) {
        id.remove(QLatin1String("ktp://"));
        id.replace(QLatin1String("/"), QLatin1String("_"));
        QString email = id;
        email.remove(0,id.indexOf(QLatin1String("?"))+1);
        id.remove(id.indexOf(QLatin1String("?")),id.length());
        path += QDir::separator() + id + QDir::separator() + email;
        QDir dir(path);
  
        if(dir.exists()) {
            QStringList ss;
            foreach(QString s, dir.entryList()) {
                if(s.compare(QLatin1String(".")) && s.compare(QLatin1String("..")))
                    ss.push_back(s);
            }
            foreach(QString s, ss) {
	      

            }
            layout->addWidget(new QLabel(ss.first()));
        } else {
            layout->addWidget(new QLabel("Chats something"));
        }

//       layout->addWidget(new QLabel(id));
    } else {
        layout->addWidget(new QLabel("Chats not available"));
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
