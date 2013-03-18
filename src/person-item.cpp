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


#include "person-item.h"
#include "contact-item.h"

#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Soprano/Vocabulary/NAO>
#include <KDebug>

PersonItem::PersonItem(const QUrl &personUri)
{
    setData(personUri, PersonsModel::UriRole);
}

PersonItem::PersonItem(const Nepomuk2::Resource &person)
{
    setData(person.uri(), PersonsModel::UriRole);
    setContacts(person.property(Nepomuk2::Vocabulary::PIMO::groundingOccurrence()).toUrlList());
    kDebug() << "new person" << text() << rowCount();
}

QVariant PersonItem::queryChildrenForRole(int role) const
{
    QVariant ret;
    for (int i = 0; i < rowCount(); i++) {
        QVariant value = child(i)->data(role);
        if (!value.isNull()) {
            ret = value;
            break;
        }
    }
    return ret;
}

QVariantList PersonItem::queryChildrenForRoleList(int role) const
{
    QVariantList ret;
    for (int i = 0; i < rowCount(); i++) {
        QVariant value = child(i)->data(role);
        if (value.type() == QVariant::List) {
            ret += value.toList();
        } else if (!value.isNull()) {
            ret += value;
        }
    }
    return ret;
}

QVariant PersonItem::data(int role) const
{
    switch(role) {
        case PersonsModel::NameRole:
        case Qt::DisplayRole: {
            QVariant value = queryChildrenForRole(Qt::DisplayRole);
            if (value.isNull()) {
                value = queryChildrenForRole(PersonsModel::ContactIdRole);
            }
            if (value.isNull()) {
                return QString("PIMO:Person - %1").arg(data(PersonsModel::UriRole).toString());
            } else {
                return value;
            }
        }
        case PersonsModel::StatusRole: //TODO: use a better algorithm for finding the actual status
        case PersonsModel::NickRole:
        case PersonsModel::LabelRole:
        case PersonsModel::IMRole:
        case PersonsModel::IMAccountTypeRole:
        case PersonsModel::PhoneRole:
        case PersonsModel::EmailRole:
        case PersonsModel::ContactIdRole:
        case PersonsModel::ContactTypeRole:
        case PersonsModel::ContactGroupsRole: {
            //we need to return empty qvariant here, otherwise we'd get a qvariant
            //with empty qvariantlist, which would get parsed as non-empty qvariant

            QVariantList val = queryChildrenForRoleList(role);
            if (val.isEmpty()) {
                return QVariant();
            } else {
                return val;
            }
        }
        case PersonsModel::PhotoRole:
            return queryChildrenForRole(role);
        case PersonsModel::ContactsCountRole:
            return rowCount();
        case PersonsModel::ResourceTypeRole:
            return PersonsModel::Person;
    }

    return QStandardItem::data(role);
}

void PersonItem::removeContacts(const QList<QUrl> &contacts)
{
    kDebug() << "remove contacts" << contacts;
    for (int i = 0; i < rowCount(); ) {
        QStandardItem *item = child(i);
        if (item && contacts.contains(item->data(PersonsModel::UriRole).toUrl())) {
            model()->invisibleRootItem()->appendRow(takeRow(i));
        } else {
            ++i;
        }
    }
    emitDataChanged();
}

void PersonItem::addContacts(const QList<QUrl> &_contacts)
{
    QList<QUrl> contacts(_contacts);
    //get existing child-contacts and remove them from the new ones
    QVariantList uris = queryChildrenForRoleList(PersonsModel::UriRole);
    foreach (const QVariant &uri, uris) {
        contacts.removeOne(uri.toUrl());
    }

    //query the model for the contacts, if they are present, then need to be just moved
    QList<QStandardItem*> toplevelContacts;
    foreach (const QUrl &uri, contacts) {
        QModelIndex contactIndex = qobject_cast<PersonsModel*>(model())->indexForUri(uri);
        if (contactIndex.isValid()) {
             toplevelContacts.append(qobject_cast<PersonsModel*>(model())->takeRow(contactIndex.row()));
        }
    }

    //append the moved contacts to this person and remove them from 'contacts'
    //so they are not added twice
    foreach (QStandardItem *contactItem, toplevelContacts) {
        ContactItem *contact = dynamic_cast<ContactItem*>(contactItem);
        appendRow(contact);
        contacts.removeOne(contact->uri());
    }

    kDebug() << "add contacts" << contacts;
    QList<ContactItem*> rows;
    foreach (const QUrl &uri, contacts) {
        ContactItem *item = new ContactItem(uri);
        item->loadData();
        appendRow(item);
    }
    emitDataChanged();
}

void PersonItem::setContacts(const QList<QUrl> &contacts)
{
    kDebug() << "set contacts" << contacts;
    if (contacts.isEmpty()) {
        //nothing to do here
        return;
    }

    if (hasChildren()) {
        QList<QUrl> toRemove;
        QVariantList uris = queryChildrenForRoleList(PersonsModel::UriRole);
        foreach (const QVariant &contact, uris) {
            if (!contacts.contains(contact.toUrl()))
                toRemove += contact.toUrl();
        }
        removeContacts(toRemove);
    }

    QList<QUrl> toAdd;
    foreach (const QUrl &contact, contacts) {
        toAdd += contact;
    }
    addContacts(toAdd);
    Q_ASSERT(hasChildren());
}
