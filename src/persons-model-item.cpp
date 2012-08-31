/*
    Persons Model Item
    Represents one person in the model
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


#include "persons-model-item.h"
#include "persons-model-contact-item.h"

#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Soprano/Vocabulary/NAO>

PersonsModelItem::PersonsModelItem(const QUrl &personUri)
{
    setData(personUri, PersonsModel::UriRole);
}

QVariant PersonsModelItem::queryChildrenForRole(int role) const
{
    for (int i = 0; i < rowCount(); i++) {
        QVariant value = child(i)->data(role);
        if (!value.isNull()) {
            return value;
        }
    }
    return QVariant();
}

QVariantList PersonsModelItem::queryChildrenForRoleList(int role) const
{
    QVariantList ret;
    for (int i = 0; i < rowCount(); i++) {
        QVariant value = child(i)->data(role);
        if (!value.isNull()) {
            ret += value;
        }
    }
    return ret;
}

QVariant PersonsModelItem::data(int role) const
{
    switch(role) {
        case PersonsModel::NameRole:
        case Qt::DisplayRole: {
            QVariant value = queryChildrenForRole(Qt::DisplayRole);
            if(value.isNull())
                return QString("PIMO:Person - %1").arg(data(PersonsModel::UriRole).toString());
            else
                return value;
        }
        case PersonsModel::NickRole:
            return queryChildrenForRole(role);
        case PersonsModel::IMRole:
        case PersonsModel::PhoneRole:
        case PersonsModel::EmailRole:
        case PersonsModel::PhotoRole:
        case PersonsModel::ContactIdRole:
            return queryChildrenForRoleList(role);
        case PersonsModel::ContactsCount:
            return rowCount();
    }

    return QStandardItem::data(role);
}
