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

#ifndef PERSONSSORTFILTERMODEL_H
#define PERSONSSORTFILTERMODEL_H

#include <QScopedPointer>
#include <QSortFilterProxyModel>
#include <kpeople/kpeople_export.h>

namespace KPeople
{

class PersonsSortFilterProxyModelPrivate;

/**
 * Helps filtering and sorting PresonsModel
 *
 * Especially useful for creating interfaces around specific properties rather
 * than the complete set as a whole.
 *
 * @sa PersonsModel
 */
class KPEOPLE_EXPORT PersonsSortFilterProxyModel : public QSortFilterProxyModel
{
Q_OBJECT
/** Specifies the properties that should be provided by the contact for the contact to be shown. */
Q_PROPERTY(QStringList requiredProperties READ requiredProperties WRITE setRequiredProperties)
public:
    PersonsSortFilterProxyModel(QObject *parent = Q_NULLPTR);
    ~PersonsSortFilterProxyModel();

    QStringList requiredProperties() const;
    void setRequiredProperties(const QStringList &props);

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(PersonsSortFilterProxyModel)

    QScopedPointer<PersonsSortFilterProxyModelPrivate> const d_ptr;
    Q_DECLARE_PRIVATE(PersonsSortFilterProxyModel);
};

}

#endif // PERSONSSORTFILTERMODEL_H
