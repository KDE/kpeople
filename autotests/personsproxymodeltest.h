/*
 * Copyright (C) 2015 Aleix Pol i Gonzalez <aleixpol@blue-systems.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef PERSONSPROXYMODELTEST_H
#define PERSONSPROXYMODELTEST_H

#include <QObject>
#include <QTemporaryFile>

class FakeContactSource;
namespace KPeople { class PersonsModel; }

class PersonsProxyModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testFiltering();

private:
    FakeContactSource *m_source;
    KPeople::PersonsModel* m_model;
    QTemporaryFile m_database;
};

#endif // PERSONSMODELTEST_H