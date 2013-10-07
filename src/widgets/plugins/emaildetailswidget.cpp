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
#include "persondata.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>

#include <KPluginFactory>
#include <KLocalizedString>

K_PLUGIN_FACTORY( EmailDetailsWidgetFactory, registerPlugin<EmailDetailsWidget>(); )
K_EXPORT_PLUGIN( EmailDetailsWidgetFactory("emaildetailswidgetplugin", "libkpeople") )

using namespace KPeople;

EmailDetailsWidget::EmailDetailsWidget(QWidget *parent, const QVariantList &args)
    : AbstractPersonDetailsWidget(parent)
{
    Q_UNUSED(args);
    setTitle(i18n("Email"));
    setIcon(QIcon::fromTheme("mail-message"));
    QVBoxLayout *newLayout = new QVBoxLayout(this);
    newLayout->setContentsMargins(0,0,0,0);
    setLayout(newLayout);
}

void EmailDetailsWidget::setPerson(PersonData *person)
{
    if(person->emails().size() == 0) {
        setActive(false);
    } else {
        setActive(true);
    }

    QLayoutItem *child;
    while ((child = layout()->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }

    Q_FOREACH (const QString &email, person->emails()) {
        QLabel *emailLabel = new QLabel(email, this);
        emailLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        layout()->addWidget(emailLabel);
    }
}
