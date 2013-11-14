/*
    Copyright (C) 2011  Martin Klapetek <mklapetek@kde.org>
    Copyright (C) 2013  David Edmundson <davidedmundsonk@kde.org>

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

#include "persondetailsview.h"

#include <QFormLayout>
#include <QLabel>
#include <QDebug>
#include <QList>

#include <KLocalizedString>

#include "abstractfieldwidgetfactory.h"
#include "plugins/emaildetailswidget.h"

namespace KPeople {

class PersonDetailsViewPrivate{
public:
    PersonData  *m_person;
    QFormLayout *m_mainLayout;
    QList<AbstractFieldWidgetFactory*> m_plugins;
};
}

using namespace KPeople;

class CoreFieldsPlugin : public AbstractFieldWidgetFactory
{
public:
    CoreFieldsPlugin(KABC::Field *field);
    virtual ~CoreFieldsPlugin();
    virtual QString label() const;
    virtual int sortWeight() const;
    virtual QWidget* createDetailsWidget(const KABC::Addressee &person, QWidget *parent) const;
private:
    KABC::Field* m_field;
};

CoreFieldsPlugin::CoreFieldsPlugin(KABC::Field* field):
    m_field(field)
{
}

CoreFieldsPlugin::~CoreFieldsPlugin()
{

}

QString CoreFieldsPlugin::label() const
{
    return m_field->label();
}

int CoreFieldsPlugin::sortWeight() const
{
    return m_field->category()*10;
}

QWidget* CoreFieldsPlugin::createDetailsWidget(const KABC::Addressee &person, QWidget *parent) const
{
    //don't handle emails here - KABC::Field just lists one which is rubbish. Instead use a custom plugin that lists everything
    if (m_field->category() == KABC::Field::Email) {
        return 0;
    }

    const QString &text = m_field->value(person);
    if (text.isEmpty()) {
        return 0;
    }
    return new QLabel(text, parent);
}


PersonDetailsView::PersonDetailsView(QWidget *parent)
    : QWidget(parent),
      d_ptr(new PersonDetailsViewPrivate())
{
    Q_D(PersonDetailsView);
    d->m_mainLayout = new QFormLayout(this);
    d->m_person = 0;

    //create plugins
    Q_FOREACH(KABC::Field *field, KABC::Field::allFields()) {
        d->m_plugins << new CoreFieldsPlugin(field);
    }

    d->m_plugins << new EmailFieldsPlugin();

    //TODO Sort plugins
}

PersonDetailsView::~PersonDetailsView()
{
    Q_D(PersonDetailsView);
//     qDeleteAll<AbstractFieldWidgetFactory>(d->m_plugins);
    delete d_ptr;
}

void PersonDetailsView::setPerson(PersonData *person)
{
    Q_D(PersonDetailsView);
    if (!d->m_person) {
        disconnect(d->m_person, SIGNAL(dataChanged()), this, SLOT(reload()));
    }

    d->m_person = person;
    connect(d->m_person, SIGNAL(dataChanged()), this, SLOT(reload()));
    reload();
}

// void PersonDetailsView::setPersonsModel(PersonsModel *model)
// {
//     Q_D(PersonDetailsView);
//     Q_FOREACH (AbstractPersonDetailsWidget *detailsWidget, d->m_detailWidgets) {
//         detailsWidget->setPersonsModel(model);
//     }
// }

void PersonDetailsView::reload()
{
    Q_D(PersonDetailsView);

    //delete everything currently in the layout
    QLayoutItem *child;
    while ((child = layout()->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }

    Q_FOREACH(AbstractFieldWidgetFactory *widgetFactory, d->m_plugins) {
        const QString label = widgetFactory->label() + ':';
        QWidget *widget = widgetFactory->createDetailsWidget(d->m_person->person(), this);

        if (widget) {
            QFont font = widget->font();
            font.setBold(true);
            widget->setFont(font);
            d->m_mainLayout->addRow(label, widget);
        }
    }
}
