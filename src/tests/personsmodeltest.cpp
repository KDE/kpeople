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

#include "personsmodeltest.h"
#include <persons-model.h>
#include <persons-model-item.h>
#include <persons-model-contact-item.h>

#include <qtest_kde.h>
#include <Nepomuk/Vocabulary/NCO>
#include <QStandardItemModel>

QTEST_KDEMAIN_CORE( PersonsModelTest )

PersonsModelTest::PersonsModelTest(QObject* parent): QObject(parent)
{}

void PersonsModelTest::testInit()
{
    QBENCHMARK {
        PersonsModel m;
        QTest::kWaitForSignal(&m, SIGNAL(peopleAdded()));
    }
}

void PersonsModelTest::testPhotos()
{
    PersonsModel m;
    QTest::kWaitForSignal(&m, SIGNAL(peopleAdded()));
    QBENCHMARK {
        for(int i=0; i<m.rowCount(); ++i) {
            QModelIndex idx = m.index(i, 0);
            idx.data(PersonsModel::PhotoRole);
        }
    }
}