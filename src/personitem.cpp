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


#include "personitem.h"
#include "contactitem.h"

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
    if (role == PersonsModel::PresenceTypeRole) {
        QVariantList presences = queryChildrenForRoleList(PersonsModel::PresenceTypeRole);
        //FIXME? this is convenient, but in the worst case it has to go through the list 6x and do lots of comparisons
        //       possible optimization would be to go through it once and find the "most online value", this would need
        //       helper function with priorities for each string presence
        if (presences.contains("available")) {
            return QVariant("available");
        }
        if (presences.contains("away")) {
            return QVariant("away");
        }
        if (presences.contains("busy") || presences.contains("dnd")) {
            return QVariant("busy");
        }
        if (presences.contains("xa")) {
            return QVariant("xa");
        }
        if (presences.contains("unknown")) {
            return QVariant("unknown");
        }

        return QVariant("offline");
    }

    if (role == PersonsModel::UriRole) {
        //uri for PersonItem is set using setData(...), so we need to query it from there
        return QStandardItem::data(role);
    }

    QVariantList ret;
    for (int i = 0; i < rowCount(); i++) {
        QVariant value;
        //if we're being asked for the child contacts uris,
        //we need to query the children for their UriRole
        if (role == PersonsModel::ChildContactsUriRole) {
            value = child(i)->data(PersonsModel::UriRole);
        } else {
            value = child(i)->data(role);
        }
        //these roles must return single QVariant
        if ((role == Qt::DisplayRole || role == Qt::DecorationRole) && !value.isNull()) {
            return value;
        }
        if (value.type() == QVariant::List) {
            ret += value.toList();
        } else if (!value.isNull()) {
            ret += value;
        }
    }

    if (ret.isEmpty()) {
        //we need to return empty qvariant here, otherwise we'd get a qvariant
        //with empty qvariantlist, which would get parsed as non-empty qvariant
        return QVariant();
    }

    return ret;
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
        //FIXME: we need to remove the fake person item here
        ContactItem *contact = dynamic_cast<ContactItem*>(contactItem);
        appendRow(contact);
        contacts.removeOne(contact->uri());
    }

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
