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
#include <TelepathyQt/Presence>

#include <KTp/types.h>

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

public:
    PersonsPresenceModel(QObject *parent = 0);
    virtual ~PersonsPresenceModel();

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant dataForContactId(const QString &contactId, int role = Qt::DisplayRole) const;

private Q_SLOTS:
    void onAccountManagerReady(Tp::PendingOperation *op);
    void onContactChanged();
    void onContactInvalidated();
    void onAllKnownContactsChanged(const Tp::Contacts &contactsAdded, const Tp::Contacts &contactsRemoved);

private:
    QVariantList queryChildrenForData(const QModelIndex &index, int role) const;

    KTp::GlobalContactManager *m_contactManager;
    Tp::AccountManagerPtr m_accountManager;
    QHash<QString, KTp::ContactPtr> m_contacts;
};

#endif // PERSONS_PRESENCE_MODEL_H
