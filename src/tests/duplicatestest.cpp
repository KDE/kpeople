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

#include "duplicatestest.h"
#include <duplicatesfinder.h>
#include <persons-model.h>
#include <person-item.h>
#include <contact-item.h>

#include <qtest_kde.h>
#include <Nepomuk2/Vocabulary/NCO>
#include <QStandardItemModel>

QTEST_KDEMAIN_CORE( DuplicatesTest )
Q_DECLARE_METATYPE(QList<PersonItem*>);
Q_DECLARE_METATYPE(QList<ContactItem*>);
Q_DECLARE_METATYPE(QList<Match>);



DuplicatesTest::DuplicatesTest(QObject* parent): QObject(parent)
{}

void DuplicatesTest::testDuplicates()
{
    QFETCH(QList<PersonItem*>, people);
    QFETCH(int, matches);
    PersonsModel m(0, false);
    foreach(PersonItem* item, people)
        m.appendRow(item);

    QScopedPointer<DuplicatesFinder> f(new DuplicatesFinder(&m));
    f->start();
    QTest::kWaitForSignal(f.data(), SIGNAL(finished(KJob*)));
    QCOMPARE(f->results().size(), matches);
}

PersonItem* createPerson1Contact(PersonsModel::ContactType t, const QString& id, const QString& nick=QString())
{
    PersonItem* ret = new PersonItem(QString("test:/%1").arg(qrand()));
    ContactItem* contact = new ContactItem(QUrl("test:/"+id+QString::number(qrand())));
    int role;
    switch(t) {
        case PersonsModel::IM: role = PersonsModel::IMAccountRole; break;
        case PersonsModel::Email: role = PersonsModel::EmailRole; break;
        case PersonsModel::Phone: role = PersonsModel::PhoneRole; break;
        default: Q_ASSERT(false && "dude!");
    }
    
    contact->setContactData(role, id);
    if(!nick.isEmpty())
        contact->setContactData(PersonsModel::NickRole, nick);
    else
        contact->setContactData(PersonsModel::NickRole, id);
    
    Q_ASSERT(contact->data(PersonsModel::ContactTypeRole).toInt()==t);
    ret->appendRow(contact);
    return ret;
}

void DuplicatesTest::testDuplicates_data()
{
    QTest::addColumn<QList<PersonItem*> >("people");
    QTest::addColumn<int>("matches");

    QList<ContactItem*> emptyContacts;
    QTest::newRow("empty") << QList<PersonItem*>() << 0;
    QTest::newRow("diffemails") << (QList<PersonItem*>()
            << createPerson1Contact(PersonsModel::Email, "a@a.es") << createPerson1Contact(PersonsModel::Email, "a@a.com")
        ) << 0;
    QTest::newRow("emails") << (QList<PersonItem*>()
            << createPerson1Contact(PersonsModel::Email, "a@a.com") << createPerson1Contact(PersonsModel::Email, "a@a.com")
        ) << 1;
}
