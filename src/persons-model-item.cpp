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
#include "person-cache-item.h"
#include "persons-model-contact-item.h"

#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NCO>
#include <Soprano/Vocabulary/NAO>

class PersonsModelItemPrivate {
public:
    QUrl personUri;
    QString displayName;
};

//-----------------------------------------------------------------------------

PersonsModelItem::PersonsModelItem(const QUrl &personUri)
    : d_ptr(new PersonsModelItemPrivate)
{
    Q_D(PersonsModelItem);

    d->personUri = personUri;
}

PersonsModelItem::~PersonsModelItem()
{

}

QVariant PersonsModelItem::data(int role) const
{
    Q_D(const PersonsModelItem);

    int i;
    PersonsModelContactItem *contact;

    switch(role) {
        case Qt::DisplayRole:
            if (d->displayName.isEmpty()) {
                //if the pimo:person has no prefLabel, loop through contacts and use first found prefLabel
                for (i = 0; i < size(); i++) {
                    contact = qobject_cast<PersonsModelContactItem*>(childAt(i));
                    QString contactDisplayName = contact->data(Qt::DisplayRole).toString();
                    if (!contactDisplayName.isEmpty()) {
                        return contactDisplayName;
                    }
                }
            }
//             if (!d->person->data(Soprano::Vocabulary::NAO::prefLabel()).isEmpty()) {
//                 return d->person->data(Soprano::Vocabulary::NAO::prefLabel());
//             } else if (!d->person->data(Nepomuk::Vocabulary::NCO::imNickname()).isEmpty()) {
//                 return d->person->data(Nepomuk::Vocabulary::NCO::imNickname());
//             }
        return QLatin1String("PIMO:Person");
    }

    return QVariant();
}

bool PersonsModelItem::setData(int role, const QVariant& value)
{
    return false;
}

#include "persons-model-item.moc"
