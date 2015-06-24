/*
    Copyright (C) 2015 Aleix Pol i Gonzalez <aleixpol@blue-systems.com>

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

#include "personssortfilterproxymodel.h"
#include "backends/abstractcontact.h"
#include "personsmodel.h"

namespace KPeople
{

class PersonsSortFilterProxyModelPrivate
{
public:
    QStringList m_keys;
};

PersonsSortFilterProxyModel::PersonsSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , d_ptr(new PersonsSortFilterProxyModelPrivate)
{}

PersonsSortFilterProxyModel::~PersonsSortFilterProxyModel()
{}

QStringList PersonsSortFilterProxyModel::requiredProperties() const
{
    Q_D(const PersonsSortFilterProxyModel);
    return d->m_keys;
}

void PersonsSortFilterProxyModel::setRequiredProperties(const QStringList &props)
{
    Q_D(PersonsSortFilterProxyModel);
    d->m_keys = props;
    invalidate();
}

bool PersonsSortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_D(const PersonsSortFilterProxyModel);
    const QModelIndex idx = sourceModel()->index(source_row, 0, source_parent);
    Q_ASSERT(idx.isValid());

    const AbstractContact::Ptr contact = idx.data(KPeople::PersonsModel::PersonVCardRole).value<AbstractContact::Ptr>();
    Q_ASSERT(contact);
    foreach(const QString &key, d->m_keys) {
        if (contact->customProperty(key).isNull())
            return false;
    }

    return true;
}

}
