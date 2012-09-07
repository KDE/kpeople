/*
    Persons Model Contact Item
    Represents person's contact item in the model
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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
#include <Nepomuk2/Vocabulary/NCO>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Nepomuk2/ResourceManager>

class PersonsModelContactItemPrivate {
public:
    QHash<QUrl, QVariant> data;
};

PersonsModelContactItem::PersonsModelContactItem(const QUrl& uri, const QString& displayName)
    : d_ptr(new PersonsModelContactItemPrivate)
{
    setData(uri, PersonsModel::UriRole);
    setText(displayName);
    setType(PersonsModel::MobilePhone);
}

PersonsModelContactItem::~PersonsModelContactItem()
{
    delete d_ptr;
}

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

void PersonsModelContactItem::addData(const QUrl &key, const QVariant &value)
{
    if(value.isNull())
        return;
    
    if(Nepomuk2::Vocabulary::NCO::imNickname() == key) {
        setText(value.toString());
    } else if (Nepomuk2::Vocabulary::NCO::imID() == key) {
        setType(PersonsModel::IM);
    } else if (Nepomuk2::Vocabulary::NCO::hasEmailAddress() == key) {
        setType(PersonsModel::Email);
    }else if (Nepomuk2::Vocabulary::NCO::emailAddress() == key) {
        setText(value.toString());
    }

    Q_D(PersonsModelContactItem);
//     kDebug() << "Inserting" << "[" << key << "]" << value;
    d->data.insert(key, value);
    emitDataChanged();
}

QVariant PersonsModelContactItem::dataValue(const QUrl &key)
{
    Q_D(PersonsModelContactItem);
    return d->data.value(key);
}

QUrl PersonsModelContactItem::uri() const
{
    return data(PersonsModel::UriRole).toUrl();
}

void PersonsModelContactItem::setType(PersonsModel::ContactType type)
{
    setData(type, PersonsModel::ContactTypeRole);
    refreshIcon();
}

QVariant PersonsModelContactItem::data(int role) const
{
    Q_D(const PersonsModelContactItem);
    switch(role) {
        case PersonsModel::NickRole: return d->data.value(Nepomuk2::Vocabulary::NCO::imNickname());
        case PersonsModel::PhoneRole: return d->data.value(Nepomuk2::Vocabulary::NCO::phoneNumber());
        case PersonsModel::EmailRole: return d->data.value(Nepomuk2::Vocabulary::NCO::emailAddress());
        case PersonsModel::IMRole: return d->data.value(Nepomuk2::Vocabulary::NCO::imID());
        case PersonsModel::PhotoRole: return d->data.value(Nepomuk2::Vocabulary::NCO::photo());
        case PersonsModel::ContactIdRole: {
            int role = -1;
            switch((PersonsModel::ContactType) data(PersonsModel::ContactTypeRole).toInt()) {
                case PersonsModel::IM: role = PersonsModel::IMRole; break;
                case PersonsModel::Phone: role = PersonsModel::PhoneRole; break;
                case PersonsModel::Email: role = PersonsModel::EmailRole; break;
                case PersonsModel::MobilePhone: role = PersonsModel::PhoneRole; break;
                case PersonsModel::Postal: role = -1; break;
            }
            if(role>=0)
                return data(role);
        }   break;
    }
    return QStandardItem::data(role);
}
