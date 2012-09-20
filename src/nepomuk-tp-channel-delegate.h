/*
    Basic Telepathy chat starter
    Copyright (C) 2011 Martin Klapetek <martin.klapetek@gmail.com>

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


#ifndef NEPOMUK_TP_CHANNEL_DELEGATE_H
#define NEPOMUK_TP_CHANNEL_DELEGATE_H

#include <QObject>

#include <TelepathyQt/AccountManager>
#include <TelepathyQt/Types>

#include "kpeople_export.h"

namespace Tp {
class PendingOperation;
class PendingOperation;
}

class NepomukTpChannelDelegatePrivate;

class KPEOPLE_EXPORT NepomukTpChannelDelegate : public QObject
{
    Q_OBJECT
public:
    explicit NepomukTpChannelDelegate(QObject* parent = 0);
    ~NepomukTpChannelDelegate();

    void startIM(const QString& accountId, const QString& contactId, const QUrl& capability);

private Q_SLOTS:
    void onAccountManagerReady(Tp::PendingOperation *op);

private:
    NepomukTpChannelDelegatePrivate * d_ptr;
    Q_DECLARE_PRIVATE(NepomukTpChannelDelegate)
};

#endif // NEPOMUK_TP_CHANNEL_DELEGATE_H
