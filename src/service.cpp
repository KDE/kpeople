/*
    D-Bus Lookup Service
    Copyright (C) 2015  Martin Klapetek <mklapetek@kde.org>

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

#include "service.h"
#include "personsmodel.h"
#include "backends/abstractcontact.h"

#include <QCoreApplication>
#include <QDBusConnection>

using namespace KPeople;

class KPeople::LookupServicePrivate
{
public:
    PersonsModel *model;
};

LookupService::LookupService(QObject *parent)
    : QDBusAbstractAdaptor(parent),
      d_ptr(new LookupServicePrivate)
{
    Q_D(LookupService);

    d->model = new PersonsModel(this);
}

QString LookupService::contactIdForContactProperty(const QString &contact, const QString &hint)
{
    Q_D(LookupService);

    for (int i = 0; i < d->model->rowCount(); i++) {
        KPeople::AbstractContact::Ptr person = d->model->data(d->model->index(i), PersonsModel::PersonVCardRole).value<KPeople::AbstractContact::Ptr>();

        if (hint == QLatin1String("phone")) {
            QVariantList dataList = person->customProperty(AbstractContact::AllPhoneNumbersProperty).toList();
            Q_FOREACH (const QVariant &data, dataList) {
                if (data.toString().contains(contact)) {
                    return d->model->data(d->model->index(i), PersonsModel::PersonUriRole).toString();
                }
            }
        }
    }

    return QString();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QDBusConnection::sessionBus().registerService(QStringLiteral("org.kde.KPeople"));
     //QDBusAbstractAdaptor crashes with 0 parent, so pass the &app
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/LookupService"), new LookupService(&app), QDBusConnection::ExportAllSlots);


    return app.exec();
}

