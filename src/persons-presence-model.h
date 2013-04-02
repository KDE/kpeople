/*
    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>

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


#ifndef PERSONS_PRESENCE_MODEL_H
#define PERSONS_PRESENCE_MODEL_H

#include <QIdentityProxyModel>

#include <TelepathyQt/Types>

#include "kpeople_export.h"

namespace KTp {
class GlobalContactManager;
}

namespace Tp {
class PendingOperation;
}

class KPEOPLE_EXPORT PersonsPresenceModel : public QIdentityProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* sourceModel READ sourceModel WRITE setSourceModel)

public:
    PersonsPresenceModel(QObject *parent = 0);
    virtual ~PersonsPresenceModel();

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant dataForContactId(const QString &contactId, int role = Qt::DisplayRole) const;
    Tp::AccountManagerPtr accountManager() const;

private Q_SLOTS:
    void onAccountManagerReady(Tp::PendingOperation *op);
    void onContactChanged();
    void onContactInvalidated();

private:
    Q_PRIVATE_SLOT(d, void onAllKnownContactsChanged(const Tp::Contacts &contactsAdded, const Tp::Contacts &contactsRemoved) )

    QVariantList queryChildrenForData(const QModelIndex &index, int role) const;
    QString queryNepomukForAccountId(const QString &contactId) const;

    class Private;
    Private* d;
};

#endif // PERSONS_PRESENCE_MODEL_H
