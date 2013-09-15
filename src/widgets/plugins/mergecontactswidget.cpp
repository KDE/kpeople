/*
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

#include "mergecontactswidget.h"
#include "personpresentationwidget.h"
#include "persondata.h"
#include "personsmodel.h"
#include "duplicatesfinder_p.h"

#include <QObject>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <KLocalizedString>
#include <KStandardDirs>
#include <KJob>
#include <KDebug>
#include <KPluginFactory>

K_PLUGIN_FACTORY( MergeContactsWidgetFactory, registerPlugin<MergeContactsWidget>(); )
K_EXPORT_PLUGIN( MergeContactsWidgetFactory("mergecontactswidgetplugin") )

using namespace KPeople;

MergeContactsWidget::MergeContactsWidget(QWidget *parent, const QVariantList &args)
    : AbstractPersonDetailsWidget(parent)
    , m_person(0)
    , m_model(0)
    , m_containerListDetails(0)
    , m_duplicatesBuster(0)
{
    Q_UNUSED(args);
    setLayout(new QVBoxLayout());
    m_mergeButton = new QPushButton(this);
    m_mergeButton->setText(i18n("Show Merge Suggestions..."));
    m_mergeButton->setVisible(false);

    connect(m_mergeButton, SIGNAL(clicked(bool)), this, SLOT(onMergePossibilitiesButtonPressed()));
    layout()->addWidget(m_mergeButton);
}

void MergeContactsWidget::setPerson(PersonData *person)
{
    m_person = person;
    searchForDuplicates();
}

void MergeContactsWidget::setPersonsModel(PersonsModel *model)
{
    m_model = model;
    searchForDuplicates();
}

void MergeContactsWidget::fillDuplicatesWidget(const QList<QPersistentModelIndex> &duplicates)
{
    // clean the previous list
    delete m_containerListDetails;
    m_listMergeContacts.clear();
    m_mergeButton->setVisible(!duplicates.isEmpty());

    // 1- Vertical list of person-presentation-widget : one contact, one checkbox
    m_containerListDetails = new QWidget(this);
    m_containerListDetails->setLayout(new QVBoxLayout());
    layout()->addWidget(m_containerListDetails);
    m_containerListDetails->setVisible(false);

    if (!duplicates.size()) {
        return ;
    }

    // building the new button
    QPushButton *triggerButton = new QPushButton(m_containerListDetails);
    triggerButton->setText(i18n("Merge with Selected Contacts"));
    connect(triggerButton, SIGNAL(clicked(bool)), this, SLOT(onMergeButtonPressed()));
    m_containerListDetails->layout()->addWidget(triggerButton);

    // building personPresentationWidget to fill up the list
    Q_FOREACH (const QPersistentModelIndex &duplicate, duplicates)
    {
        // displaying contact in a user friendly way
        kDebug() << "Name retireved form the duplicate :" << duplicate.data(Qt::DisplayRole).toString();

        QIcon avatar ;
        QString name = duplicate.data(Qt::DisplayRole).toString();

        QVariant decoration = duplicate.data(Qt::DecorationRole);
        if (decoration.type() == (QVariant::Icon)) {
            avatar = decoration.value<QIcon>();

        } else if (decoration.type() == (QVariant::Pixmap)) {
            avatar = QIcon(decoration.value<QPixmap>());
        }

        // memorise the link between checkbox widget and model index
        PersonPresentationWidget *myMergeContactWidget = new PersonPresentationWidget(name, avatar, m_containerListDetails);
        m_containerListDetails->layout()->addWidget(myMergeContactWidget);
        m_listMergeContacts.append(qMakePair(duplicate, myMergeContactWidget));
    }
}

QList<QPersistentModelIndex> MergeContactsWidget::duplicateBusterFromPerson(const QUrl &uri) const
{
    Q_ASSERT(m_duplicatesBuster);
    QList<Match> wrongFormatResults = m_duplicatesBuster->results();
    QList<QPersistentModelIndex> duplicateMatching;

    Q_FOREACH (const Match &match, wrongFormatResults) {
        // pick up only the couple with match with our parameter index
        QUrl uriA = match.indexA.data(PersonsModel::UriRole).toUrl();

        // Tested with URI because QModelIndex isn't reliable
        if (uriA == uri) {
            duplicateMatching.append(match.indexB);
        }
        QUrl uriB = match.indexB.data(PersonsModel::UriRole).toUrl();
        if (uriB == uri) {
            duplicateMatching.append(match.indexA);
        }
    }
    kDebug() << "Result of the duplicates Buster :" << duplicateMatching.size();
    return duplicateMatching;
}

void MergeContactsWidget::searchForDuplicates()
{
    m_mergeButton->setVisible(false);
    if (m_duplicatesBuster || !m_person || !m_person->isValid() || !m_model) {
        kDebug() << "Merge Widget failed to launch the duplicates search";
        return;
    }
    m_duplicatesBuster = new DuplicatesFinder(m_model , this);
    connect (m_duplicatesBuster, SIGNAL(result(KJob*)), SLOT(searchForDuplicatesFinished()));
    m_duplicatesBuster->setSpecificPerson(m_person->uri());
    m_duplicatesBuster->start();
}

void MergeContactsWidget::searchForDuplicatesFinished()
{
    QList<QPersistentModelIndex> duplicates = duplicateBusterFromPerson(m_person->uri() );

    fillDuplicatesWidget(duplicates);
    m_duplicatesBuster = 0;
}

void MergeContactsWidget::onMergePossibilitiesButtonPressed()
{
    m_mergeButton->setVisible(false);
    m_containerListDetails->setVisible(true);
}

void MergeContactsWidget::onMergeButtonPressed()
{
    // Retrieve the selected contacts
    QList<QUrl> urisToMerge;
    urisToMerge << m_person->uri();

    // do the merge
    Q_FOREACH (const QPersistentModelIndex &pIndex, getContactsCheckedToMerge()) {
        urisToMerge << pIndex.data(PersonsModel::UriRole).toUrl();
    }
    m_model->createPersonFromUris(urisToMerge);
    searchForDuplicates();
}

QList<QPersistentModelIndex> MergeContactsWidget::getContactsCheckedToMerge() const
{
    QList<QPersistentModelIndex> indexesToMerge;

    // retrieve all the widget where the box is checked
    QPair<QPersistentModelIndex, PersonPresentationWidget*> mergeContact ;
    Q_FOREACH (mergeContact, m_listMergeContacts) {
        if (mergeContact.second->isContactSelected()) {
            indexesToMerge.append(mergeContact.first);
        }
    }
    kDebug() << "Amount of checked box enable :" << indexesToMerge.size();
    return indexesToMerge;
}
