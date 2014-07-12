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

#ifndef PERSONEVENTVIEW_H
#define PERSONEVENTVIEW_H

#include <QWidget>
#include <QVariantList>

#include "../kpeople_export.h"

#include "../persondata.h"

class QLabel;
class QVBoxLayout;

namespace KPeople {
class PersonEventViewPrivate;

class KPEOPLE_EXPORT PersonEventView: public QWidget
{
    Q_OBJECT
public:
    PersonEventView(QWidget *parent = 0);
    virtual ~PersonEventView();

public Q_SLOTS:
    void setPerson(PersonData *person);

private Q_SLOTS:
    void reload();

private:
    Q_DECLARE_PRIVATE(PersonEventView)
    PersonEventViewPrivate* d_ptr;
};
}

#endif // PERSONEVENTVIEW_H