/*
    Persons Model
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

#ifndef PERSONSMODELTEST_H
#define PERSONSMODELTEST_H

#include <QtCore/QObject>

class PersonsModelTest : public QObject
{
    Q_OBJECT
    public:
        explicit PersonsModelTest(QObject* parent = 0);

    private slots:
        void testInit();
        void testPhotos();
        void testActions();
};

#endif // PERSONSMODELTEST_H
