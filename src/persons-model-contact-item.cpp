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
#include <Nepomuk2/Vocabulary/NIE>
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

PersonsModelContactItem::PersonsModelContactItem(const QUrl& uri)
    : d_ptr(new PersonsModelContactItemPrivate)
{
    setData(uri, PersonsModel::UriRole);
    setType(PersonsModel::MobilePhone);
}

PersonsModelContactItem::PersonsModelContactItem(const Nepomuk2::Resource& contact)
    : d_ptr(new PersonsModelContactItemPrivate)
{
    setData(contact.uri(), PersonsModel::UriRole);
    pullResourceProperties(contact);

    QUrl val = d_ptr->data.value(Nepomuk2::Vocabulary::NCO::hasIMAccount()).toUrl();
    if(val.isValid()) pullResourceProperties(Nepomuk2::Resource(val));

    val = d_ptr->data.value(Nepomuk2::Vocabulary::NCO::photo()).toUrl();
    if(val.isValid()) pullResourceProperties(Nepomuk2::Resource(val));
}

PersonsModelContactItem::~PersonsModelContactItem()
{
    delete d_ptr;
}

void PersonsModelContactItem::pullResourceProperties(const Nepomuk2::Resource& res)
{
    QHash<QUrl, Nepomuk2::Variant> props = res.properties();
    for(QHash<QUrl, Nepomuk2::Variant>::const_iterator it=props.constBegin(), itEnd=props.constEnd(); it!=itEnd; ++it) {
        addData(it.key(), it->variant());
    }
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

    Q_D(PersonsModelContactItem);
    if (Nepomuk2::Vocabulary::NCO::imID() == key) {
        setType(PersonsModel::IM);
    } else if (Nepomuk2::Vocabulary::NCO::emailAddress() == key) {
        setType(PersonsModel::Email);
    }

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
        case Qt::DisplayRole:
            if (!data(PersonsModel::NickRole).toString().isEmpty()) {
                return data(PersonsModel::NickRole);
            }
            if (!data(PersonsModel::EmailRole).toString().isEmpty()) {
                return data(PersonsModel::EmailRole);
            }
            if (!data(PersonsModel::PhoneRole).toString().isEmpty()) {
                return data(PersonsModel::PhoneRole);
            }
            return QString("Unknown person"); //FIXME: temporary
        case PersonsModel::NickRole: return d->data.value(Nepomuk2::Vocabulary::NCO::imNickname());
        case PersonsModel::PhoneRole: return d->data.value(Nepomuk2::Vocabulary::NCO::phoneNumber());
        case PersonsModel::EmailRole: return d->data.value(Nepomuk2::Vocabulary::NCO::emailAddress());
        case PersonsModel::IMRole: return d->data.value(Nepomuk2::Vocabulary::NCO::imID());
        case PersonsModel::PhotoRole: return d->data.value(Nepomuk2::Vocabulary::NIE::url());
        case PersonsModel::IMAccountUriRole: return d->data.value(Nepomuk2::Vocabulary::NCO::hasIMAccount());
        case PersonsModel::StatusRole: return d->data.value(Nepomuk2::Vocabulary::NCO::imStatus());
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
        case PersonsModel::ResourceTypeRole:
            return PersonsModel::Contact;
    }
    return QStandardItem::data(role);
}

void PersonsModelContactItem::modifyData(const QUrl& name, const QVariantList& added)
{
    Q_D(PersonsModelContactItem);
    d->data[name] = added;
    emitDataChanged();
}

void PersonsModelContactItem::modifyData(const QUrl& name, const QVariant& newValue)
{
    Q_D(PersonsModelContactItem);
    kDebug() << "Old data:" << d->data[name];
    d->data[name] = newValue;
    kDebug() << "New Data:" << d->data[name];
    emitDataChanged();
}


void PersonsModelContactItem::removeData(const QUrl& uri)
{
    Q_D(PersonsModelContactItem);
    d->data.remove(uri);
    emitDataChanged();
}
