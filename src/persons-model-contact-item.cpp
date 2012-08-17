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
    QString displayName;
    QString contactId;
    PersonsModel::ContactType type;

    QUrl uri;
    QMultiHash<QUrl, QString> data;
};

PersonsModelContactItem::PersonsModelContactItem(const QString &displayName, const QString &contactId, PersonsModel::ContactType type)
    : d_ptr(new PersonsModelContactItemPrivate)
{
    Q_D(PersonsModelContactItem);

    d->displayName = displayName;
    d->contactId = contactId;
    d->type = type;
}

PersonsModelContactItem::~PersonsModelContactItem()
{

}

QVariant PersonsModelContactItem::data(int role) const
{
    Q_D(const PersonsModelContactItem);
    switch (role) {
        case Qt::DisplayRole:
            if (d->displayName.isEmpty()) {
                if (!d->data.value(Nepomuk::Vocabulary::NCO::imNickname()).isEmpty()) {
                    const_cast<PersonsModelContactItemPrivate*>(d)->displayName = d->data.value(Nepomuk::Vocabulary::NCO::imNickname());
                    return d->displayName;
                }
            }
            return d->displayName;
        case PersonsModel::ContactTypeRole:
            return d->type;
        case Qt::DecorationRole:
            if (d->type == PersonsModel::Email) {
                return KIcon(QLatin1String("mail-mark-unread"));
            } else if (d->type == PersonsModel::IM) {
                //FIXME: return the account string to have account icons
                return KIcon(QLatin1String("im-user"));
            }
        case PersonsModel::ContactIdRole:
            return d->contactId;
    }
    return QVariant();
}

void PersonsModelContactItem::addData(const QUrl &key, const QString &value)
{
    Q_D(PersonsModelContactItem);
    kDebug() << "Inserting" << value << "(" << key << ")";
    d->data.insert(key, value);
}

void PersonsModelContactItem::addData(const QUrl &key, const QStringList &values)
{
    Q_D(PersonsModelContactItem);
    Q_FOREACH (const QString &value, values) {
        kDebug() << "Inserting (multi)" << value << "(" << key << ")";
        d->data.insert(key, value);
    }
}

QString PersonsModelContactItem::data(const QUrl &key)
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
    Q_D(PersonsModelContactItem);
    d->type = type;

}
