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

class PersonsModelContactItemPrivate {
public:
    QString displayName;
    QString contactId;
    PersonsModel::ContactType type;
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

bool PersonsModelContactItem::setData(int role, const QVariant& value)
{
    return false;
}

#include "persons-model-contact-item.moc"
