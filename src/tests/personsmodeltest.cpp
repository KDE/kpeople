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
#include <personsmodel.h>
#include <personitem.h>
#include <contactitem.h>
#include <personactionsmodel.h>

#include <qtest_kde.h>
#include <Nepomuk2/Vocabulary/NCO>
#include <QStandardItemModel>

QTEST_KDEMAIN_CORE( PersonsModelTest )

PersonsModelTest::PersonsModelTest(QObject* parent): QObject(parent)
{}

void PersonsModelTest::testInit()
{
    QBENCHMARK {
        PersonsModel m(0, PersonsModel::FeatureIM);
        QTest::kWaitForSignal(&m, SIGNAL(modelInitialized()));
    }
}

void PersonsModelTest::testPhotos()
{
    PersonsModel m(PersonsModel::FeatureAvatars, 0);
    QTest::kWaitForSignal(&m, SIGNAL(modelInitialized()));
    int count = 0;
    QBENCHMARK {
        for(int i=0; i<m.rowCount(); ++i) {
            QModelIndex idx = m.index(i, 0);
            QVariant ret = idx.data(PersonsModel::PhotoRole);
            count += ret.toList().size();
        }
    }
    QVERIFY(count>0); //there should be someone with photos...
}

void PersonsModelTest::testActions()
{
    PersonsModel m(PersonsModel::FeatureEmails, 0);
    QTest::kWaitForSignal(&m, SIGNAL(modelInitialized()));
    for(int i=0; i<m.rowCount(); ++i) {
        PersonActionsModel a;
        a.setPerson(&m, i);

        if (a.rowCount(QModelIndex()) == 0) {
            qDebug() << "error: " << i << m.index(i, 0).data();
        }
        QVERIFY(a.rowCount(QModelIndex())>0);
    }
}
