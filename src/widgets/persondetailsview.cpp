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

#include <KLocalizedString>
#include <KStandardDirs>
#include <KService>
#include <KServiceTypeTrader>
#include <KPluginInfo>
#include <KPluginLoader>
#include <KPluginFactory>

#include "abstractfieldwidgetfactory.h"
#include "plugins/emaildetailswidget.h"
#include "global.h"

#include "ui_person-details-presentation.h"

namespace KPeople {

class PersonDetailsViewPrivate{
public:
    PersonData  *m_person;
    Ui::PersonDetailsPresentation *m_personDetailsPresentation;
    QWidget *m_mainWidget;
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
    virtual QWidget* createDetailsWidget(const KABC::Addressee &person, const KABC::AddresseeList &contacts, QWidget *parent) const;
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

QWidget* CoreFieldsPlugin::createDetailsWidget(const KABC::Addressee &person, const KABC::AddresseeList &contacts, QWidget *parent) const
{
    Q_UNUSED(contacts)
    //don't handle emails here - KABC::Field just lists one which is rubbish. Instead use a custom plugin that lists everything
    if (m_field->category() & KABC::Field::Email) {
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
    Q_FOREACH(KABC::Field *field, KABC::Field::allFields()) {
        d->m_plugins << new CoreFieldsPlugin(field);
    }

    d->m_plugins << new EmailFieldsPlugin();

    // load every KPeopleWidgets Plugin
    KService::List pluginList = KServiceTypeTrader::self()->query( QLatin1String("KPeopleWidgets/Plugin"));

    QList<KPluginInfo> plugins = KPluginInfo::fromServices(pluginList);

    Q_FOREACH(const KPluginInfo &p, plugins) {
        QString error;
        AbstractFieldWidgetFactory *f = p.service()->createInstance<AbstractFieldWidgetFactory>(this, QVariantList(), &error);
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
    dynamic_cast<QVBoxLayout*>(layout())->insertWidget(layoutIndex, d->m_mainWidget);

    QFormLayout *layout = new QFormLayout(d->m_mainWidget);
    layout->setSpacing(4);

    //update header information
    //FIXME - possibly split this out into a new class with a nice setPerson method

    QPixmap avatar;

    if (!d->m_person->person().photo().data().isNull()) {
        avatar = QPixmap::fromImage(d->m_person->person().photo().data());
    } else if (!d->m_person->person().photo().url().isEmpty()) {
        avatar = QPixmap(d->m_person->person().photo().url());
    } else {
        avatar = QPixmap(KStandardDirs::locate("data", "kpeople/dummy_avatar.png"));
    }

    QString contactPresence = d->m_person->person().custom(QLatin1String("telepathy"), QLatin1String("presence"));

    d->m_personDetailsPresentation->avatarPixmapLabel->setPixmap(avatar.scaled(96, 96, Qt::KeepAspectRatio)); //FIXME
    d->m_personDetailsPresentation->presencePixmapLabel->setPixmap(QIcon::fromTheme(KPeople::iconNameForPresenceString(contactPresence)).pixmap(32, 32)); //FIXME
    d->m_personDetailsPresentation->nameLabel->setText(d->m_person->person().formattedName());

    Q_FOREACH(AbstractFieldWidgetFactory *widgetFactory, d->m_plugins) {
        const QString label = widgetFactory->label() + ':';
        QWidget *widget = widgetFactory->createDetailsWidget(d->m_person->person(), d->m_person->contacts(), this);

        if (widget) {
            QFont font = widget->font();
            font.setBold(true);
            widget->setFont(font);
            QLabel *widgetLabel = new QLabel(label, this);
            layout->addRow(widgetLabel, widget);
        }
    }
}
