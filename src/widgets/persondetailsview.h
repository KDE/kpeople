/*
    Copyright (C) 2011  Martin Klapetek <mklapetek@kde.org>

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



#ifndef PERSON_DETAILS_VIEW_H
#define PERSON_DETAILS_VIEW_H

#include <QWidget>
#include <QVariantList>

#include "../kpeople_export.h"

#include "../persondata.h"

class QLabel;
class QVBoxLayout;

namespace KPeople {
class PersonDetailsViewPrivate;

class KPEOPLE_EXPORT PersonDetailsView : public QWidget
{
    Q_OBJECT
public:
    PersonDetailsView(QWidget *parent = 0);
    virtual ~PersonDetailsView();

public Q_SLOTS:
    void setPerson(PersonData *person);
//     void setPersonsModel(PersonsModel *model);

private Q_SLOTS:
    void reload();

private:
    Q_DECLARE_PRIVATE(PersonDetailsView)
    PersonDetailsViewPrivate* d_ptr;
};
}

#endif // PERSON_DETAILS_VIEW_H
