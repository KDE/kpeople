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

#include "emaildetailswidget.h"

#include <QVBoxLayout>
#include <QLabel>

#include <KPluginFactory>
#include <KLocalizedString>

// K_PLUGIN_FACTORY( EmailDetailsWidgetFactory, registerPlugin<EmailDetailsWidget>(); )
// K_EXPORT_PLUGIN( EmailDetailsWidgetFactory("emaildetailswidgetplugin") )

using namespace KPeople;


QWidget* EmailFieldsPlugin::createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList &contacts, QWidget* parent) const
{
    Q_UNUSED(contacts);
    if (person.emails().isEmpty()) {
        return 0;
    }
    QWidget *widget = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0,0,0,0);
    Q_FOREACH(const QString &email, person.emails()) {
        layout->addWidget(new QLabel(email));
    }
    widget->setLayout(layout);
    return widget;
}

QString EmailFieldsPlugin::label() const
{
    return i18n("Email");
}

int EmailFieldsPlugin::sortWeight() const
{
    return 50;
}

