/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2014  <copyright holder> <email>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CHAT_H
#define CHAT_H
#include "abstractfieldwidgetfactory.h"
#include <KDebug>
#include <KTp/Logger/pending-logger-dates.h>
#include <KTp/Logger/pending-logger-operation.h>

using namespace KPeople;


class Chat : public AbstractFieldWidgetFactory
{
    Q_OBJECT
public:
    explicit Chat(QObject* parent = 0);
    virtual QString label() const;
    virtual int sortWeight() const;
    virtual QWidget* createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList& contacts, QWidget* parent) const;

private slots:
    void onPendingDates(KTp::PendingLoggerOperation*);
    void onEventsFinished(KTp::PendingLoggerOperation*);

};

#endif // CHAT_H
