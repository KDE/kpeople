/*
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

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



#include "phonedetailswidget.h"
#include "persondata.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

#include <KLocalizedString>
#include <KPluginFactory>

K_PLUGIN_FACTORY( PhoneDetailsWidgetFactory, registerPlugin<PhoneDetailsWidget>(); )
K_EXPORT_PLUGIN( PhoneDetailsWidgetFactory("phonedetailswidgetplugin"))

using namespace KPeople;

PhoneDetailsWidget::PhoneDetailsWidget(QWidget *parent, const QVariantList &args):
    AbstractPersonDetailsWidget(parent)
{
    Q_UNUSED(args);
    setTitle(i18n("Phone"));
    setIcon(QIcon::fromTheme("phone"));

    setLayout(new QVBoxLayout(this));
}

void PhoneDetailsWidget::setPerson(PersonData *person)
{
    if (person->phones().size() == 0) {
        setActive(false);
    } else {
        setActive(true);
    }

    QLayoutItem *child;
    while ((child = layout()->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }

    Q_FOREACH (const QString &phone, person->phones()) {
        QLabel *phoneLabel = new QLabel(phone, this);
        phoneLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        layout()->addWidget(phoneLabel);
    }
}
