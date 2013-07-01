/*
    KPeople - Duplicates
    Copyright (C) 2013  Franck Arrecot <franck.arrecot@gmail.com>

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
#include <QApplication>

#include <personsmodel.h>
#include <personsmodelfeature.h>
#include <widgets/mergedialog.h>

using namespace KPeople;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    MergeDialog dialog;
    PersonsModel *persons = new PersonsModel(&dialog);
    QList<KPeople::PersonsModelFeature> features;
    features << KPeople::PersonsModelFeature::emailModelFeature(true)
             << KPeople::PersonsModelFeature::avatarModelFeature(true)
             << KPeople::PersonsModelFeature::imModelFeature(true)
             << KPeople::PersonsModelFeature::fullNameModelFeature(true);
    persons->startQuery(features);

    dialog.setPersonsModel(persons);

    QObject::connect(persons, SIGNAL(modelInitialized()), &dialog, SLOT(show()));
    QObject::connect(&dialog, SIGNAL(finished(int)), &app, SLOT(quit()));
    app.exec();
}
