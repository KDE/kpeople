/*
    Persons Model Contact Item
    Represents person's contact item in the model
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>

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


#include "persons-model-contact-item.h"
#include <KIcon>
#include <KDebug>
#include <Nepomuk/Vocabulary/NCO>
#include <Soprano/Vocabulary/NAO>

class PersonsModelContactItemPrivate {
public:
    QUrl uri;
    QMultiHash<QUrl, QString> data;
};

PersonsModelContactItem::PersonsModelContactItem(const QUrl& uri, const QString &displayName, const QString &contactId, PersonsModel::ContactType type)
    : d_ptr(new PersonsModelContactItemPrivate)
{
    Q_D(PersonsModelContactItem);
    d->uri = uri;

    setData(contactId, PersonsModel::ContactIdRole);
    setData(type, PersonsModel::ContactTypeRole);
    setText(displayName);
    
    refreshIcon();
}

PersonsModelContactItem::~PersonsModelContactItem()
{}

QMap<PersonsModel::ContactType, QIcon> initializeTypeIcons()
{
    QMap<PersonsModel::ContactType, QIcon> ret;
    ret.insert(PersonsModel::Email, QIcon::fromTheme(QLatin1String("mail-mark-unread")));
    ret.insert(PersonsModel::IM, QIcon::fromTheme(QLatin1String("im-user")));
    ret.insert(PersonsModel::Phone, QIcon::fromTheme(QLatin1String("phone")));
    ret.insert(PersonsModel::MobilePhone, QIcon::fromTheme(QLatin1String("phone")));
    ret.insert(PersonsModel::Postal, QIcon::fromTheme(QLatin1String("mail-message")));
    return ret;
}

static QMap<PersonsModel::ContactType, QIcon> s_contactTypeMap = initializeTypeIcons();

void PersonsModelContactItem::refreshIcon()
{
    PersonsModel::ContactType type = (PersonsModel::ContactType) data(PersonsModel::ContactTypeRole).toInt();
    setIcon(s_contactTypeMap[type]);
}

void PersonsModelContactItem::addData(const QUrl &key, const QString &value)
{
    if(value.isEmpty())
        return;
    
    if(Nepomuk::Vocabulary::NCO::imNickname() == key)
        setText(value);

    Q_D(PersonsModelContactItem);
    kDebug() << "Inserting" << value << "(" << key << ")";
    d->data.insert(key, value);
}

void PersonsModelContactItem::addData(const QUrl &key, const QStringList &values)
{
    if(values.isEmpty())
        return;

    Q_D(PersonsModelContactItem);
    Q_FOREACH (const QString &value, values) {
        kDebug() << "Inserting (multi)" << value << "(" << key << ")";
        d->data.insert(key, value);
    }
}

QString PersonsModelContactItem::dataValue(const QUrl &key)
{
    Q_D(PersonsModelContactItem);
    return d->data.value(key);
}

QMultiHash<QUrl, QString> PersonsModelContactItem::dataHash() const
{
    Q_D(const PersonsModelContactItem);
    return d->data;
}

QUrl PersonsModelContactItem::uri() const
{
    Q_D(const PersonsModelContactItem);
    return d->uri;
}

void PersonsModelContactItem::setType (PersonsModel::ContactType type)
{
    setData(type, PersonsModel::ContactTypeRole);
    refreshIcon();
}
