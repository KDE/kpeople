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


#ifndef PERSONS_MODEL_CONTACT_ITEM_H
#define PERSONS_MODEL_CONTACT_ITEM_H

#include "kpeople_export.h"
#include <QStandardItem>
#include "persons-model.h"

class QAction;
class PersonsModelContactItemPrivate;

class KPEOPLE_EXPORT PersonsModelContactItem : public QStandardItem
{
public:
    /**
     * @param displayName - What will be visible to the user (used as Qt::DisplayRole)
     * @param type - What kind of contact this is
     */
    explicit PersonsModelContactItem(const QUrl &uri);
    explicit PersonsModelContactItem(const Nepomuk2::Resource &res);
    virtual ~PersonsModelContactItem();

    QUrl uri() const;

    void addData(const QUrl &key, const QVariant &value);
    void modifyData(const QUrl &name, const QVariantList &newValue);
    void modifyData(const QUrl &name, const QVariant &newValue);
    void setType(PersonsModel::ContactType type);

    QVariantList dataValue(const QUrl &key);
    virtual QVariant data(int role = Qt::UserRole + 1) const;
    void removeData(const QUrl &uri);
    void pullResourceProperties(const Nepomuk2::Resource &res);

private:
    void refreshIcon();

    PersonsModelContactItemPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(PersonsModelContactItem)
};

Q_DECLARE_METATYPE(PersonsModelContactItem*);
Q_DECLARE_METATYPE(QList<QAction *>);

#endif // PERSONS_MODEL_CONTACT_ITEM_H
