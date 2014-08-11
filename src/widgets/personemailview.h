/*
    Copyright 2014  Nilesh Suthar <nileshsuthar@live.in>

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

#ifndef PERSON_EMAILS_VIEW_H
#define PERSON_EMAILS_VIEW_H

#include <QWidget>

#include "../kpeople_export.h"
#include "../persondata.h"

class QLabel;
class QVBoxLayout;

namespace KPeople {
class PersonEmailsViewPrivate;

class KPEOPLE_EXPORT PersonEmailsView : public QWidget
{
    Q_OBJECT
public:
    PersonEmailsView(QWidget *parent = 0);
    virtual ~PersonEmailsView();

public Q_SLOTS:
    void setPerson(PersonData *person);

private Q_SLOTS:
    void reload();

private:
    Q_DECLARE_PRIVATE(PersonEmailsView)
    PersonEmailsViewPrivate *d_ptr;
};
}

#endif // PERSON_EMAILS_VIEW_H
