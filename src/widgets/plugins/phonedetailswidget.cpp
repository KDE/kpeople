/*
    SPDX-FileCopyrightText: 2013 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "phonedetailswidget.h"
#include "persondata.h"

#include <QVBoxLayout>
#include <QLabel>

#include <KLocalizedString>
#include <KPluginFactory>

K_PLUGIN_FACTORY(PhoneDetailsWidgetFactory, registerPlugin<PhoneDetailsWidget>();)
K_EXPORT_PLUGIN(PhoneDetailsWidgetFactory("phonedetailswidgetplugin"))

using namespace KPeople;

PhoneDetailsWidget::PhoneDetailsWidget(QWidget *parent, const QVariantList &args):
    AbstractPersonDetailsWidget(parent)
{
    Q_UNUSED(args);
    setTitle(i18nc("Phone details title", "Phone"));
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

    const auto lst = person->phones();
    for (const QString &phone : lst) {
        QLabel *phoneLabel = new QLabel(phone, this);
        phoneLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        layout()->addWidget(phoneLabel);
    }
}
