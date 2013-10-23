/*
 *    Persons Model Contact Item
 *    Represents person's contact item in the model
 *    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>
 *    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 2.1 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "contactitem_p.h"
#include "personpluginmanager.h"
#include "basepersonsdatasource.h"
#include "personitem_p.h"

#include <QAction>

#include <KIcon>
#include <KDebug>

#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Nepomuk2/ResourceManager>

#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

using namespace KPeople;

class ContactItemPrivate {
public:
    QUrl uri;
    QHash<int, QVariant> data;

    bool isBeingUpdated;

    ///@returns a list of the roles that we'll expect to be a QVariantList
    static QSet<int> listRoles() {
        static QSet<int> s_listRoles;
        if(s_listRoles.isEmpty()) {
            s_listRoles.insert(PersonsModel::FullNamesRole);
            s_listRoles.insert(PersonsModel::EmailsRole);
            s_listRoles.insert(PersonsModel::NicknamesRole);
            s_listRoles.insert(PersonsModel::PhonesRole);
//disable IM roles from being a list.
//On a nepomuk level a contact can have two IM accounts
//However KTp will always feed one per contact
//Multiples break the presence lookup
//             s_listRoles.insert(PersonsModel::IMsRole);
            s_listRoles.insert(PersonsModel::PhotosRole);
            s_listRoles.insert(PersonsModel::GroupsRole);
        }
        return s_listRoles;
    }
};

ContactItem::ContactItem(const QUrl &uri)
: d_ptr(new ContactItemPrivate)
{
    //     setData(uri, PersonsModel::UriRole);
    d_ptr->uri = uri;
    d_ptr->isBeingUpdated = false;
}

ContactItem::~ContactItem()
{
    delete d_ptr;
}

void ContactItem::setContactData(int role, const QVariant &v)
{
    Q_D(ContactItem);

    if (v.isNull()) {
        //don't insert empty values, in case of list roles this
        //would insert QVariant(QVariantList(QVariant())),
        //empty QVariant will be returned from data(...) instead
        return;
    }

    //look if we already have data for the passed role
    QHash<int, QVariant>::iterator it = d->data.find(role);

    //if we're inserting data for a role that needs to be a list, make it a list
    QVariant value = d->listRoles().contains(role) ? (QVariantList() << v) : v;

    if (it == d->data.end()) {  //we don't have data for this role yet
        d->data.insert(role, value);
    } else if (*it != value) {  //if the stored value is not the same...
        Q_ASSERT(value.type() == it->type());
        *it = value;            //...overwrite it
    }

    emitDataChanged();
}

//TODO update so value can be a list. This will then concat onto any existing list.

void ContactItem::addContactData(int role, const QVariant &value)
{
    Q_D(ContactItem);
    if (value.isNull()) {
        //don't insert empty values, in case of list roles this
        //would insert QVariant(QVariantList(QVariant())),
        //empty QVariant will be returned from data(...) instead
        return;
    }

    //look if we already have data for the passed role
    QHash<int, QVariant>::iterator it = d->data.find(role);
    //if we're not inserting data for a role that's supposed to be a list,
    //overwrite it with setContactData instead
    if (!d->listRoles().contains(role)) {
        setContactData(role, value);
    } else {
        if (it == d->data.end()) {  //if we don't have data for the passed role yet
            d->data.insert(role, QVariantList() << value);  //insert new data as QVariantList
        } else if (*it != value) {  //check if we're not inserting the same value
            Q_ASSERT(it->type() == QVariant::List);
            QVariantList current = it->toList();
            Q_ASSERT(current.isEmpty() || current.first().type() == value.type());
            current.append(value);  //append to the stored list
            *it = current;
        }

        emitDataChanged();
    }
}

void ContactItem::removeData(int role)
{
    Q_D(ContactItem);
    if (d->data.remove(role)) {
        emitDataChanged();
    }
}


QUrl ContactItem::uri() const
{
    Q_D(const ContactItem);
    return d->uri;
}

QVariant ContactItem::data(int role) const
{
    Q_D(const ContactItem);

    QHash<int, QVariant>::const_iterator it = d->data.constFind(role);
    if (it!=d->data.constEnd()) {
        return *it;
    }

    switch(role) {
        case Qt::DisplayRole:
            if (!data(PersonsModel::NicknamesRole).isNull()) {
                const QVariantList &r = data(PersonsModel::NicknamesRole).toList();
                if (!r.isEmpty()) {
                    return r.first();
                }
            }
            if (!data(PersonsModel::FullNamesRole).isNull()) {
                const QVariantList &r = data(PersonsModel::FullNamesRole).toList();
                if (!r.isEmpty()) {
                    return r.first();
                }
            }
            if (!data(PersonsModel::EmailsRole).isNull()) {
                const QVariantList r = data(PersonsModel::EmailsRole).toList();
                if (!r.isEmpty()) {
                    return r.first();
                }
            }
            if (!data(PersonsModel::IMsRole).isNull()) {
                return data(PersonsModel::IMsRole);
            }
            if (!data(PersonsModel::PhonesRole).isNull()) {
                const QVariantList r = data(PersonsModel::PhonesRole).toList();
                if (!r.isEmpty()) {
                    return r.first();
                }
            }

            return i18n("Unknown contact");
        case Qt::DecorationRole: {
            const QVariantList photos = d->data.value(PersonsModel::PhotosRole).toList();
            return photos.isEmpty() ? KIcon("im-user") : KIcon(photos.first().toUrl().toLocalFile());
        }
        case PersonsModel::UriRole:
            return d->uri;
        case PersonsModel::PresenceTypeRole:
        case PersonsModel::PresenceDisplayRole:
        case PersonsModel::PresenceDecorationRole:
        case PersonsModel::PresenceIconNameRole:
            return PersonPluginManager::presencePlugin()->dataForContact(data(PersonsModel::IMsRole).toString(), role);
    }

    return QStandardItem::data(role);
}

void ContactItem::loadData(PersonsModel* m)
{
    Q_D(ContactItem);

    if (d->isBeingUpdated) {
        kDebug() << "Contact already being updated";
        return;
    }

    d->data.clear();

    Q_ASSERT(m); //crash if this contact is not part of the model

    d->isBeingUpdated = true;
    m->updateContact(this);
}

void ContactItem::clear()
{
    Q_D(ContactItem);

    d->data.clear();
}

void ContactItem::finishLoadingData()
{
    Q_D(ContactItem);
    d->isBeingUpdated = false;
    emitDataChanged();
    dynamic_cast<PersonItem*>(parent())->contactDataChanged();
}
