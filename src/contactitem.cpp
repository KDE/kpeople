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

class ContactItemPrivate {
public:
    QUrl uri;
    QHash<int, QVariant> data;

    ///@returns a list of the roles that we'll expect to be a QVariantList
    static QSet<int> listRoles() {
        static QSet<int> s_listRoles;
        if(s_listRoles.isEmpty()) {
            //TODO: document this in personsmodel.h
            s_listRoles.insert(PersonsModel::PhotoRole);
            s_listRoles.insert(PersonsModel::EmailRole);
            s_listRoles.insert(PersonsModel::PhoneRole);
        }
        return s_listRoles;
    }
};

ContactItem::ContactItem(const QUrl &uri)
    : d_ptr(new ContactItemPrivate)
{
//     setData(uri, PersonsModel::UriRole);
    d_ptr->uri = uri;
}

ContactItem::~ContactItem()
{
    delete d_ptr;
}

void ContactItem::setContactData(int role, const QVariant &v)
{
    Q_D(ContactItem);
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
            if (!data(PersonsModel::NickRole).toString().isEmpty()) {
                return data(PersonsModel::NickRole);
            }
            if (!data(PersonsModel::LabelRole).toString().isEmpty()) {
                return data(PersonsModel::LabelRole);
            }
            if (!data(PersonsModel::NameRole).toString().isEmpty()) {
                return data(PersonsModel::NameRole);
            }
            if (!data(PersonsModel::IMRole).toString().isEmpty()) {
                return data(PersonsModel::IMRole);
            }
            if (!data(PersonsModel::EmailRole).toString().isEmpty()) {
                return data(PersonsModel::EmailRole);
            }
            if (!data(PersonsModel::PhoneRole).toString().isEmpty()) {
                return data(PersonsModel::PhoneRole);
            }

            return QString("Unknown person"); //FIXME: temporary
        case PersonsModel::UriRole: return d->uri; break;
        case PersonsModel::PresenceTypeRole:
        case PersonsModel::PresenceDisplayRole:
        case PersonsModel::PresenceDecorationRole:
            return PersonPluginManager::presencePlugin()->dataForContact(data(PersonsModel::IMRole).toString(), role);
        case PersonsModel::ContactsCountRole: return 1;
        case PersonsModel::ResourceTypeRole:
            return PersonsModel::Contact;
        case Qt::DecorationRole: {
            QVariantList photos = d->data.value(PersonsModel::PhotoRole).toList();
            return photos.isEmpty() ? KIcon("im-user") : KIcon(photos.first().toUrl().toLocalFile());
        }
    }
    return QStandardItem::data(role);
}

void ContactItem::loadData()
{
    Q_D(ContactItem);

    QHash<QString, PersonsModel::Role> bindingRoleMap;
    bindingRoleMap.insert("nco_imNickname", PersonsModel::NickRole);
    bindingRoleMap.insert("nao_prefLabel", PersonsModel::LabelRole);
    bindingRoleMap.insert("nco_imID", PersonsModel::IMRole);
    bindingRoleMap.insert("nco_imImAcountType", PersonsModel::IMAccountTypeRole);
    bindingRoleMap.insert("nco_hasIMAccount", PersonsModel::IMAccountUriRole);
    bindingRoleMap.insert("nco_contactGroupName", PersonsModel::ContactGroupsRole);
    bindingRoleMap.insert("nie_url", PersonsModel::PhotoRole);
    bindingRoleMap.insert("nco_emailAddress", PersonsModel::EmailRole);

    QString query = QString::fromUtf8(
            "select DISTINCT ?nco_hasIMAccount "
            "?nco_imNickname ?nco_imID ?nco_imAccountType ?nco_hasEmailAddress "
            "?nie_url ?nao_prefLabel ?nco_contactGroupName "

                "WHERE { "
                    "OPTIONAL { %1  nao:prefLabel  ?nao_prefLabel. }"

//                    "OPTIONAL { "
                        "%1                                    nco:hasIMAccount            ?nco_hasIMAccount. "
                        "OPTIONAL { ?nco_hasIMAccount          nco:imNickname              ?nco_imNickname. } "
                        "OPTIONAL { ?nco_hasIMAccount          nco:imID                    ?nco_imID. } "
                        "OPTIONAL { ?nco_hasIMAccount          nco:imAccountType           ?nco_imAccountType. } "
//                    " } "
                    "OPTIONAL { "
                        "%1 nco:belongsToGroup ?nco_belongsToGroup . "
                        "?nco_belongsToGroup nco:contactGroupName ?nco_contactGroupName . "
                    " }"
                    "OPTIONAL {"
                        "%1                         nco:photo                   ?phRes. "
                        "?phRes                     nie:url                     ?nie_url. "
                    " } "
                    "OPTIONAL { "
                        "%1                         nco:hasEmailAddress         ?nco_hasEmailAddress. "
                        "?nco_hasEmailAddress       nco:emailAddress            ?nco_emailAddress. "
                    " } "
                "}")
            .arg(Soprano::Node::resourceToN3(d->uri));

    Soprano::Model *model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );
    // FIXME: The result might come multiple times
    d->data.clear();
    if( it.next() ) {

        //iterate over the results and add the wanted properties into the contact
        foreach (const QString& bName, it.bindingNames()) {
            if (!bindingRoleMap.contains(bName))
                continue;

            PersonsModel::Role role = bindingRoleMap.value(bName);
            QString value = it.binding(bName).toString();
            if (!value.isEmpty()) {
                addContactData(role, value);
            }
        }
        emitDataChanged();
    }
}
