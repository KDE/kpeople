/*
 * Copyright 2014  Nilesh Suthar <nileshsuthar@live.in>
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

#ifndef EMAILS_H
#define EMAILS_H

#include "abstractfieldwidgetfactory.h"
#include "plugins/emaillistmodel.h"

class KJob;
class QModelIndex;

namespace KABC {
class Addressee;
class AddresseeList;
}
using namespace KPeople;

class Emails : public AbstractFieldWidgetFactory
{
    Q_OBJECT
public:
    explicit Emails(QObject *parent = 0);
    virtual QString label() const;
    virtual int sortWeight() const;
    virtual QWidget *createDetailsWidget(const KABC::Addressee &, const KABC::AddresseeList &, QWidget *) const;
private:
    QList<email> emailList;
    EmailListModel *me;

private Q_SLOTS:
    void onEmailDoubleClicked(const QModelIndex &);
    void jobFinished(KJob *);

};

#endif // EMAILS_H
