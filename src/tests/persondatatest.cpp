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

#include "persondatatest.h"
#include <persons-model.h>
#include <persondata.h>
#include <personactions.h>

#include <qtest_kde.h>

QTEST_KDEMAIN_CORE( PersonDataTest )

PersonDataTest::PersonDataTest(QObject* parent)
    : QObject(parent)
{
    PersonsModel m;
    QTest::kWaitForSignal(&m, SIGNAL(peopleAdded()));
    
    for(int i=0; i<m.rowCount(); i++) {
        m_contactIds.append(m.data(m.index(i,0), PersonsModel::ContactIdRole).toList().first().toString());
    }
}

void PersonDataTest::testLocalPeople()
{
    foreach(const QString& id, m_contactIds) {
        PersonData d;
        d.setContactId(id);
        QTest::kWaitForSignal(&d, SIGNAL(dataInitialized()));
        
        PersonActions actions;
        actions.setPerson(&d);
        QVERIFY(actions.rowCount()>0);
    }
}
