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

#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NCO>
#include <Soprano/Vocabulary/NAO>

PersonsModelItem::PersonsModelItem(const QUrl &personUri)
{
    setData(personUri, PersonsModel::UriRole);
}

QVariant PersonsModelItem::data(int role) const
{
    switch(role) {
        case Qt::DisplayRole:
            for (int i = 0; i < rowCount(); i++) {
                QString contactDisplayName = child(i)->data(Qt::DisplayRole).toString();
                if (!contactDisplayName.isEmpty()) {
                    return contactDisplayName;
                }
            }
            return QString(QLatin1String("PIMO:Person - %1")).arg(data(PersonsModel::UriRole).toString());
    }

    return QStandardItem::data(role);
}
