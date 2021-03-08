/*
    SPDX-FileCopyrightText: 2013 Franck Arrecot <franck.arrecot@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "mergecontactswidget.h"
#include "duplicatesfinder_p.h"
#include "persondata.h"
#include "personpresentationwidget.h"
#include "personsmodel.h"

#include <QPushButton>
#include <QVBoxLayout>

#include "kpeople_debug.h"
#include <KJob>
#include <KLocalizedString>
#include <KPluginFactory>

K_PLUGIN_FACTORY(MergeContactsWidgetFactory, registerPlugin<MergeContactsWidget>();)
K_EXPORT_PLUGIN(MergeContactsWidgetFactory("mergecontactswidgetplugin"))

using namespace KPeople;

MergeContactsWidget::MergeContactsWidget(QWidget *parent, const QVariantList &args)
    : AbstractPersonDetailsWidget(parent)
    , m_person(nullptr)
    , m_model(nullptr)
    , m_containerListDetails(nullptr)
    , m_duplicatesBuster(nullptr)
{
    Q_UNUSED(args);

    auto *layout = new QVBoxLayout(this);
    m_mergeButton = new QPushButton(this);
    m_mergeButton->setText(i18n("Show Merge Suggestions..."));
    m_mergeButton->setVisible(false);

    connect(m_mergeButton, SIGNAL(clicked(bool)), this, SLOT(onMergePossibilitiesButtonPressed()));
    layout->addWidget(m_mergeButton);
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
    auto *listDetailsLayout = new QVBoxLayout{m_containerListDetails};

    layout()->addWidget(m_containerListDetails);
    m_containerListDetails->setVisible(false);

    if (duplicates.isEmpty()) {
        return;
    }

    // building the new button
    QPushButton *triggerButton = new QPushButton(m_containerListDetails);
    triggerButton->setText(i18n("Merge with Selected Contacts"));
    connect(triggerButton, SIGNAL(clicked(bool)), this, SLOT(onMergeButtonPressed()));
    listDetailsLayout->addWidget(triggerButton);

    // building personPresentationWidget to fill up the list
    Q_FOREACH (const QPersistentModelIndex &duplicate, duplicates) {
        // displaying contact in a user friendly way
        qCDebug(KPEOPLE_LOG) << "Name retireved form the duplicate :" << duplicate.data(Qt::DisplayRole).toString();

        QIcon avatar;
        QString name = duplicate.data(Qt::DisplayRole).toString();

        QVariant decoration = duplicate.data(Qt::DecorationRole);
        if (decoration.type() == (QVariant::Icon)) {
            avatar = decoration.value<QIcon>();

        } else if (decoration.type() == (QVariant::Pixmap)) {
            avatar = QIcon(decoration.value<QPixmap>());
        }

        // memorise the link between checkbox widget and model index
        PersonPresentationWidget *myMergeContactWidget = new PersonPresentationWidget(name, avatar, m_containerListDetails);
        listDetailsLayout->addWidget(myMergeContactWidget);
        m_listMergeContacts.append(qMakePair(duplicate, myMergeContactWidget));
    }
}

QList<QPersistentModelIndex> MergeContactsWidget::duplicateBusterFromPerson(const QUrl &uri) const
{
    Q_ASSERT(m_duplicatesBuster);
    const QList<Match> wrongFormatResults = m_duplicatesBuster->results();
    QList<QPersistentModelIndex> duplicateMatching;

    for (const Match &match : wrongFormatResults) {
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
    qCDebug(KPEOPLE_LOG) << "Result of the duplicates Buster :" << duplicateMatching.size();
    return duplicateMatching;
}

void MergeContactsWidget::searchForDuplicates()
{
    m_mergeButton->setVisible(false);
    if (m_duplicatesBuster || !m_person || !m_person->isValid() || !m_model) {
        qCDebug(KPEOPLE_LOG) << "Merge Widget failed to launch the duplicates search";
        return;
    }
    m_duplicatesBuster = new DuplicatesFinder(m_model, this);
    connect(m_duplicatesBuster, SIGNAL(result(KJob *)), SLOT(searchForDuplicatesFinished()));
    m_duplicatesBuster->setSpecificPerson(m_person->uri());
    m_duplicatesBuster->start();
}

void MergeContactsWidget::searchForDuplicatesFinished()
{
    QList<QPersistentModelIndex> duplicates = duplicateBusterFromPerson(m_person->uri());

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
    const auto lst = getContactsCheckedToMerge();
    for (const QPersistentModelIndex &pIndex : lst) {
        urisToMerge << pIndex.data(PersonsModel::UriRole).toUrl();
    }
    m_model->createPersonFromUris(urisToMerge);
    searchForDuplicates();
}

QList<QPersistentModelIndex> MergeContactsWidget::getContactsCheckedToMerge() const
{
    QList<QPersistentModelIndex> indexesToMerge;

    // retrieve all the widget where the box is checked
    for (const QPair<QPersistentModelIndex, PersonPresentationWidget *> &mergeContact : qAsConst(m_listMergeContacts)) {
        if (mergeContact.second->isContactSelected()) {
            indexesToMerge.append(mergeContact.first);
        }
    }
    qCDebug(KPEOPLE_LOG) << "Amount of checked box enable :" << indexesToMerge.size();
    return indexesToMerge;
}
