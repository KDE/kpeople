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
#include <QVBoxLayout>
#include <QDebug>
#include <QList>
#include <QStandardPaths>

#include <KLocalizedString>
#include <KPluginMetaData>
#include <KPluginLoader>
#include <KPluginFactory>

#include "abstractfieldwidgetfactory.h"
#include "plugins/emaildetailswidget.h"
#include "global.h"

#include "ui_person-details-presentation.h"

namespace KPeople
{

class PersonDetailsViewPrivate
{
public:
    PersonData  *m_person;
    Ui::PersonDetailsPresentation *m_personDetailsPresentation;
    QWidget *m_mainWidget;
    QList<AbstractFieldWidgetFactory *> m_plugins;
};
}

using namespace KPeople;

class CoreFieldsPlugin : public AbstractFieldWidgetFactory
{
public:
    CoreFieldsPlugin(const QString &field);
    virtual ~CoreFieldsPlugin();
    virtual QString label() const;
    virtual int sortWeight() const;
    virtual QWidget *createDetailsWidget(const PersonData &person, QWidget *parent) const;
private:
    QString m_field;
};

CoreFieldsPlugin::CoreFieldsPlugin(const QString &field):
    m_field(field)
{
}

CoreFieldsPlugin::~CoreFieldsPlugin()
{

}

QString CoreFieldsPlugin::label() const
{
#warning fixme, should be made user-visible somehow
    return m_field;
}

int CoreFieldsPlugin::sortWeight() const
{
    return 1;
}

QWidget *CoreFieldsPlugin::createDetailsWidget(const PersonData &person, QWidget *parent) const
{
//  we have a plugin specific for e-mails.
    if (m_field == QLatin1String("email")) {
        return 0;
    }

    QString text = person.contactCustomProperty(m_field).toString();
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
    setLayout(new QVBoxLayout(this));
    d->m_mainWidget = new QWidget(this);
    d->m_person = 0;

    QWidget *details = new QWidget();
    d->m_personDetailsPresentation = new Ui::PersonDetailsPresentation();
    d->m_personDetailsPresentation->setupUi(details);
    layout()->addWidget(details);
    layout()->addWidget(d->m_mainWidget);
    layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));

    //create plugins
#warning figure out a way to list properties
    QStringList fields { QStringLiteral("name"), QStringLiteral("all-email") };
    Q_FOREACH (const QString &field, fields) {
        d->m_plugins << new CoreFieldsPlugin(field);
    }

    d->m_plugins << new EmailFieldsPlugin();

    // load every KPeopleWidgets Plugin
    QVector<KPluginMetaData> personPluginList = KPluginLoader::findPlugins(QStringLiteral("kpeople/widgets"));

    Q_FOREACH (const KPluginMetaData &service, personPluginList) {
        KPluginLoader loader(service.fileName());
        KPluginFactory *factory = loader.factory();
        AbstractFieldWidgetFactory *f = qobject_cast<AbstractFieldWidgetFactory*>(factory->create());

        Q_ASSERT(f);
        if (f) {
            d->m_plugins << f;
        }
    }

    //TODO Sort plugins
}

PersonDetailsView::~PersonDetailsView()
{
    delete d_ptr;
}

void PersonDetailsView::setPerson(PersonData *person)
{
    Q_D(PersonDetailsView);
    if (d->m_person) {
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

    //replace the entire main widget
    int layoutIndex = layout()->indexOf(d->m_mainWidget);
    layout()->takeAt(layoutIndex);
    d->m_mainWidget->deleteLater();
    d->m_mainWidget = new QWidget(this);
    dynamic_cast<QVBoxLayout *>(layout())->insertWidget(layoutIndex, d->m_mainWidget);

    QFormLayout *layout = new QFormLayout(d->m_mainWidget);
    layout->setSpacing(4);

    //update header information
    //FIXME - possibly split this out into a new class with a nice setPerson method

    QPixmap avatar = d->m_person->photo();

    d->m_personDetailsPresentation->avatarPixmapLabel->setPixmap(avatar.scaled(96, 96, Qt::KeepAspectRatio)); //FIXME
    d->m_personDetailsPresentation->presencePixmapLabel->setPixmap(QIcon::fromTheme(d->m_person->presenceIconName()).pixmap(32, 32)); //FIXME
    d->m_personDetailsPresentation->nameLabel->setText(d->m_person->name());

    Q_FOREACH (AbstractFieldWidgetFactory *widgetFactory, d->m_plugins) {
        const QString label = widgetFactory->label() + QLatin1Char(':');
        QWidget *widget = widgetFactory->createDetailsWidget(d->m_person->personUri(), this);

        if (widget) {
            QFont font = widget->font();
            font.setBold(true);
            widget->setFont(font);
            QLabel *widgetLabel = new QLabel(label, this);
            layout->addRow(widgetLabel, widget);
        }
    }
}
