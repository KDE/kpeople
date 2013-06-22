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

#include "contactitem.h"
#include "personpluginmanager.h"
#include "basepersonsdatasource.h"

#include "base-persons-data-source.h"

#include <QAction>

#include <KIcon>
#include <KDebug>
#include <KServiceTypeTrader>
#include <kdemacros.h>

#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Nepomuk2/ResourceManager>

#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

class ContactItemPrivate {
public:
    QUrl uri;
    QHash<int, QVariant> data;

    bool isBeingUpdated;

    ///@returns a list of the roles that we'll expect to be a QVariantList
    static QSet<int> listRoles() {
        static QSet<int> s_listRoles;
        if(s_listRoles.isEmpty()) {
            //TODO: document this in persons-model.h
            s_listRoles.insert(PersonsModel::PhotosRole);
            s_listRoles.insert(PersonsModel::EmailsRole);
            s_listRoles.insert(PersonsModel::PhonesRole);
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

    QHash< int, QVariant >::iterator it = d->data.find(role);
    QVariant value = d->listRoles().contains(role) ? (QVariantList() << v) : v;
    if (it == d->data.end()) {
        d->data.insert(role, value);
        emitDataChanged();
    } else if (*it != value) {
        Q_ASSERT(value.type() == it->type());
        *it = value;
        emitDataChanged();
    }
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
    QHash<int, QVariant>::iterator it = d->data.find(role);
    if (!d->listRoles().contains(role)) {
        setContactData(role, value);
    } else {
        if (it == d->data.end()) {
            d->data.insert(role, QVariantList() << value);
            emitDataChanged();
        } else if (*it != value) {
            Q_ASSERT(it->type() == QVariant::List);
            QVariantList current = it->toList();
            Q_ASSERT(current.isEmpty() || current.first().type()==value.type());
            current.append(value);
            *it = current;
            emitDataChanged();
        }
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
                return data(PersonsModel::NicknamesRole);
            }
            if (!data(PersonsModel::FullNamesRole).isNull()) {
                return data(PersonsModel::FullNamesRole);
            }
            if (!data(PersonsModel::EmailsRole).isNull()) {
                return data(PersonsModel::EmailsRole);
            }
            if (!data(PersonsModel::IMsRole).isNull()) {
                return data(PersonsModel::IMsRole);
            }
            if (!data(PersonsModel::PhonesRole).isNull()) {
                return data(PersonsModel::PhonesRole);
            }

            return QString("Unknown contact");
        case Qt::DecorationRole: {
            const QVariantList photos = d->data.value(PersonsModel::PhotosRole).toList();
            return photos.isEmpty() ? KIcon("im-user") : KIcon(photos.first().toUrl().toLocalFile());
        }
        case PersonsModel::UriRole: return d->uri; break;
        case PersonsModel::PresenceTypeRole:
        case PersonsModel::PresenceDisplayRole:
        case PersonsModel::PresenceDecorationRole:
            return PersonPluginManager::presencePlugin()->dataForContact(data(PersonsModel::IMsRole).toString(), role);
    }

    return QStandardItem::data(role);
}

void ContactItem::loadData()
{
    Q_D(ContactItem);

    if (d->isBeingUpdated) {
        kDebug() << "Contact already being updated";
        return;
    }

    d->data.clear();

    PersonsModel *m = qobject_cast<PersonsModel*>(model());

    if (m) {
        d->isBeingUpdated = true;
        m->updateContact(this);
    }
}

void ContactItem::finishLoadingData()
{
    Q_D(ContactItem);
    d->isBeingUpdated = false;
    emitDataChanged();
}
