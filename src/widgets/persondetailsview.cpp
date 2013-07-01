/*
    Copyright (C) 2011  Martin Klapetek <mklapetek@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "persondetailsview.h"

#include <Nepomuk/Resource>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Variant>
#include <KDebug>
#include <KIconLoader>
#include <KStandardDirs>
#include <KIcon>

#include <QVBoxLayout>
#include <QLabel>
#include <QAction>
#include <QToolButton>
#include <QModelIndex>

#include "persondata.h"
#include "personsmodel.h"
#include "abstractpersondetailswidget.h"

#include "plugins/emaildetailswidget.h"
#include "plugins/phonedetailswidget.h"
#include "plugins/mergecontactswidget.h"

using namespace Nepomuk::Vocabulary;
using namespace KPeople;

class DetailsGroupWidget : public QWidget
{
public:
    DetailsGroupWidget(AbstractPersonDetailsWidget *detailsWidget, QWidget *parent);
};

DetailsGroupWidget::DetailsGroupWidget(AbstractPersonDetailsWidget *detailsWidget, QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setColumnStretch(1, 1);

    QLabel *iconLabel = new QLabel(this);
    QLabel *titleLabel = new QLabel(this);

    QFont f;
    f.setPixelSize(16); //NOTE: HACK
    titleLabel->setFont(f);

    layout->addWidget(iconLabel, 0, 0);
    layout->addWidget(titleLabel, 0, 1);

    layout->addWidget(detailsWidget, 1, 1);

    titleLabel->setText(detailsWidget->title());
    iconLabel->setPixmap(detailsWidget->icon().pixmap(KIconLoader::SizeSmall, KIconLoader::SizeSmall));

    setVisible(detailsWidget->active());
    connect(detailsWidget, SIGNAL(activeChanged(bool)), SLOT(setVisible(bool)));

    setLayout(layout);
}

PersonDetailsView::PersonDetailsView(QWidget *parent)
    : QWidget(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_contactPixmap = new QLabel(this);
    m_contactNameLabel = new QLabel(this);
    QFont f;
    f.setPixelSize(18);
    m_contactNameLabel->setFont(f);
    m_contactStatusLabel = new QLabel(this);
    m_contactBirthdayLabel = new QLabel(this);

    QGridLayout *namePresenceLayout = new QGridLayout(this);
    namePresenceLayout->addWidget(m_contactPixmap, 0, 0, 2, 1);
    namePresenceLayout->addWidget(m_contactNameLabel, 0, 1);
    namePresenceLayout->addWidget(m_contactStatusLabel, 0, 2);
    namePresenceLayout->addWidget(m_contactBirthdayLabel, 1, 1);
    namePresenceLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding), 0, 3);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);

    namePresenceLayout->addWidget(line, 2, 0, 1, 4);

    m_contactsListWidget = new QWidget(this);

    m_mainLayout->addLayout(namePresenceLayout);
    m_mainLayout->addWidget(m_contactsListWidget);


    m_detailWidgets << new EmailDetailsWidget(this);
//     m_detailWidgets << new IMDetailsWidget(this);
    m_detailWidgets <<  new PhoneDetailsWidget(this);
//     m_detailWidgets << new FacebookConnector(this);
//     m_detailWidgets <<  new RecentEmailsDetailsWidget(this);
    m_detailWidgets <<  new MergeContactsWidget(this);

    Q_FOREACH (AbstractPersonDetailsWidget *detailsWidget, m_detailWidgets) {
        m_mainLayout->addWidget(new DetailsGroupWidget(detailsWidget, this));
    }

    m_mainLayout->addSpacerItem(new QSpacerItem(32, 32, QSizePolicy::Minimum, QSizePolicy::Expanding));

    setLayout(m_mainLayout);
}

PersonDetailsView::~PersonDetailsView()
{
}

void PersonDetailsView::setPerson(const PersonDataPtr &person)
{
    disconnect(m_person.data(), SIGNAL(dataChanged()), this, SLOT(reload()));

    m_person = person;
    connect(m_person.data(), SIGNAL(dataChanged()), this, SLOT(reload()));
    reload();
}

void PersonDetailsView::setPersonsModel(PersonsModel *model)
{
    Q_FOREACH (AbstractPersonDetailsWidget *detailsWidget, m_detailWidgets) {
        detailsWidget->setPersonsModel(model);
    }
}

void PersonDetailsView::reload()
{
    kDebug();
    QPixmap avatar;
    QString avatarPath = m_person->avatar().toLocalFile();
    if (!avatarPath.isEmpty()) {
        avatar.load(avatarPath);
    }

    if (avatar.isNull()) {
        //FIXME (also move inside PersonData?)
        avatar.load(KStandardDirs::locate("data", "person-viewer/dummy_avatar.png"));
    }
    m_contactPixmap->setPixmap(avatar.scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_contactNameLabel->setText(m_person->name());
    m_contactStatusLabel->setPixmap(iconForPresence(m_person->status()));
    m_contactBirthdayLabel->setText(m_person->groups().join(", "));

    Q_FOREACH (AbstractPersonDetailsWidget* detailsWidget, m_detailWidgets) {
        detailsWidget->setPerson(m_person.data());
    }
}

QPixmap PersonDetailsView::iconForPresence(const QString &presenceString)
{
    KIconLoader il;
    if (presenceString == "available") {
        return il.loadIcon("user-online", KIconLoader::Toolbar);
    }
    if (presenceString == "away") {
        return il.loadIcon("user-away", KIconLoader::Toolbar);
    }
    if (presenceString == "busy" || presenceString == "dnd") {
        return il.loadIcon("user-busy", KIconLoader::Toolbar);
    }
    if (presenceString == "xa") {
        return il.loadIcon("user-online", KIconLoader::Toolbar);
    }
    if (presenceString == "offline") {
        return il.loadIcon("user-offline", KIconLoader::Toolbar);
    }

    return QPixmap();
}
