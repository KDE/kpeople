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
#include "personsmodel.h"
#include "abstractpersonplugin.h"

#include <QStandardItem>

class QAction;
class ContactItemPrivate;

namespace KPeople
{

class KPEOPLE_EXPORT ContactItem : public QStandardItem
{
public:
    explicit ContactItem(const QUrl &uri);
    virtual ~ContactItem();

    QUrl uri() const;

    /** sets the @p value as the role */
    void setContactData(int role, const QVariant &value);

    /**
     * such the data to the role, with special treatment to list roles.
     * In such cases the data will be added to the list
     */
    void addContactData(int role, const QVariant &value);
    void removeData(int role);

    virtual QVariant data(int role = Qt::UserRole + 1) const;

    /**
     * Discards all the previous data and reload everything by
     * querying Nepomuk
     */
    void loadData(KPeople::PersonsModel* m);

    void clear();

    void finishLoadingData();

private:
    ContactItemPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(ContactItem)
};

}

Q_DECLARE_METATYPE(KPeople::ContactItem*);
Q_DECLARE_METATYPE(QList<QAction *>);

#endif // PERSONS_MODEL_CONTACT_ITEM_H
