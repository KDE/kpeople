/*
    Copyright (C) 2013  Franck Arrecot <franck.arrecot@gmail.com>

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

#include "mergecontactswidget.h"
#include "personpresentationwidget.h"

#include <QGridLayout>
#include <QObject>
#include <KDebug>

#include <KLocalizedString>
#include <KIconLoader>
#include <KStandardDirs>
#include <KIcon>
#include <KJob>

#include "persondata.h"
#include "personsmodel.h"
#include "duplicatesfinder.h"
#include "math.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>


MergeContactsWidget::MergeContactsWidget(QWidget *parent)
    : AbstractPersonDetailsWidget(parent)
    , m_model(0)
    , m_containerListDetails(0)
    , m_duplicatesBuster(0)
    , m_person(0)
{
    setLayout(new QVBoxLayout());

    m_mergeButton = new QPushButton(this);
    m_mergeButton->setText(i18n("Merge Suggestions"));
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

    // 1- Vertical list of person-presentation-widget : one contact, one checkbox
    m_containerListDetails = new QWidget(this);
    m_containerListDetails->setLayout(new QVBoxLayout());
    layout()->addWidget(m_containerListDetails);

    // building the new button
    QPushButton *triggerButton = new QPushButton(m_containerListDetails);
    triggerButton->setText(i18n("Merge with Selected Contacts", 0));
    connect(triggerButton, SIGNAL(clicked(bool)), this, SLOT(onMergeButtonPressed()));
    m_containerListDetails->layout()->addWidget(triggerButton);

    // building personPresentationWidget to fill up the list
    foreach (const QPersistentModelIndex &duplicate, duplicates)
    {
        // displaying contact in a user friendly way
        kDebug() << "Name retireved form the duplicate :" << duplicate.data(Qt::DisplayRole).toString();

        QString name = duplicate.data(Qt::DisplayRole).toString();
        QPixmap avatar;
        QString avatarPath = duplicate.data(PersonsModel::PhotosRole).toString();
        if (!avatarPath.isEmpty()) {
            avatar.load(avatarPath);
        }
        if (avatar.isNull()) {
            avatar.load(KStandardDirs::locate("data", "kpeople/dummy_avatar.png"));
        }

        // memorise the link between checkbox widget and model index
        PersonPresentationWidget *myMergeContactWidget = new PersonPresentationWidget(name, avatar, m_containerListDetails);
        m_containerListDetails->layout()->addWidget(myMergeContactWidget);
        m_listMergeContacts.append(qMakePair(duplicate, myMergeContactWidget));
    }
    m_containerListDetails->setVisible(false);
}

QList<QPersistentModelIndex> MergeContactsWidget::duplicateBusterFromPerson(const QModelIndex &original)
{
    Q_ASSERT(m_duplicatesBuster);
    QList<Match> wrongFormatResults = m_duplicatesBuster->results();
    QList<QPersistentModelIndex> duplicateMatching;

    foreach (const Match &match, wrongFormatResults) {
        // pick up only the couple with match with our parameter index
        if (match.indexA == original) {
            duplicateMatching.append(match.indexB);
        }

        if (match.indexB == original) {
            duplicateMatching.append(match.indexA);
        }
    }
    m_duplicatesBuster = 0;

    kDebug() << "Result of the duplicates Buster :" << duplicateMatching.size();
    return duplicateMatching;
}

void MergeContactsWidget::searchForDuplicates()
{
    if (m_duplicatesBuster || !m_person || !m_model) {
        return;
    }
    m_duplicatesBuster = new DuplicatesFinder(m_model , this);
    connect (m_duplicatesBuster, SIGNAL(result(KJob*)), SLOT(searchForDuplicatesFinished()));
    m_duplicatesBuster->start();
}

void MergeContactsWidget::searchForDuplicatesFinished()
{
    kDebug() << "Result From duplicatesFinder Available!" ;

    QModelIndex index = m_model->indexForUri( m_person->uri() );
    QList<QPersistentModelIndex> duplicates = duplicateBusterFromPerson(index);
    m_mergeButton->setVisible(!duplicates.isEmpty());
    fillDuplicatesWidget(duplicates);
}

void MergeContactsWidget::onMergePossibilitiesButtonPressed()
{
    m_mergeButton->setVisible(false);
    m_containerListDetails->setVisible(true);
}

void MergeContactsWidget::onMergeButtonPressed()
{
    // Retrieve the selected contacts
    QList<QPersistentModelIndex> indexesToMerge = getContactsCheckedToMerge();
    QModelIndex index = m_model->indexForUri(m_person->uri());
    indexesToMerge.prepend(index);

    // do the merge
    QList<QModelIndex> indexToMergeConverted;
    foreach (const QPersistentModelIndex &pIndex, indexesToMerge) {
        indexToMergeConverted << pIndex;
    }
    //commented because it removes the test cases
    m_model->createPersonFromIndexes(indexToMergeConverted);

    searchForDuplicates();
}

QList<QPersistentModelIndex> MergeContactsWidget::getContactsCheckedToMerge()
{
    QList<QPersistentModelIndex> indexesToMerge;

    // retrieve all the widget where the box is checked
    QPair<QPersistentModelIndex, PersonPresentationWidget*> mergeContact ;
    foreach (mergeContact, m_listMergeContacts) {
        if (mergeContact.second->isContactSelected()) {
            indexesToMerge.append(mergeContact.first);
        }
    }
    kDebug() << "Amount of checked box enable :" << indexesToMerge.size();
    return indexesToMerge;
}


