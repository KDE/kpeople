/*
    KPeople - Duplicates
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

#include "mergedialog.h"
#include "mergedelegate.h"
#include "duplicatesfinder.h"
#include "persondata.h"
#include "personsmodel.h"

#include <KDebug>

#include <QObject>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QListView>
#include <QStandardItemModel>

#include <KStandardDirs>
#include <KLocalizedString>

using namespace KPeople;

class MergeDialogPrivate {
public:
    PersonsModel *personsModel;
    QListView *view;
    MergeDelegate *delegate;

    QStandardItemModel *model;
    DuplicatesFinder *duplicatesFinder;

    QPixmap defaultPixmap;
};

//NOTE: look at KDialog
MergeDialog::MergeDialog(QWidget *parent)
    : QDialog(parent)
    , d_ptr(new MergeDialogPrivate)
{
    Q_D(MergeDialog);

    d_ptr->personsModel = 0;
    d_ptr->delegate = 0;
    d_ptr->duplicatesFinder = 0;

    setWindowTitle(i18n("Duplicates Manager"));
    d->defaultPixmap.load(KStandardDirs::locate("data", "kpeople/dummy_avatar.png"));
    setLayout(new QVBoxLayout());
    setMinimumSize(450,350);

    d->model = new QStandardItemModel(this);
    d->view = new QListView(this);
    d->view->setModel(d->model);
    d->view->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QLabel *topLabel = new QLabel(i18n("Select contacts to be merged"));

    QDialogButtonBox* buttons = new QDialogButtonBox(this);
    buttons->addButton(QDialogButtonBox::Ok);
    buttons->addButton(QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(accepted()), SLOT(onMergeButtonClicked()));
    connect(buttons, SIGNAL(rejected()), SLOT(reject()));

    layout()->addWidget(topLabel);
    layout()->addWidget(d->view);
    layout()->addWidget(buttons);
}

MergeDialog::~MergeDialog()
{
   delete d_ptr;
}

void MergeDialog::setPersonsModel(PersonsModel *model)
{
    Q_D(MergeDialog);
    d->personsModel = model;
    if (d->personsModel) {
        searchForDuplicates();
        connect(d->personsModel, SIGNAL(modelInitialized()), SLOT(searchForDuplicates()));
    }
}

void MergeDialog::searchForDuplicates()
{
    Q_D(MergeDialog);
    if (!d->personsModel || !d->personsModel->rowCount() || d->duplicatesFinder) {
        kDebug() << "MergeDialog failed to launch the duplicates research";
        return;
    }
    d->duplicatesFinder = new DuplicatesFinder(d->personsModel);
    connect (d->duplicatesFinder, SIGNAL(result(KJob*)), SLOT(searchForDuplicatesFinished(KJob*)));
    d->duplicatesFinder->start();
}

void MergeDialog::mergeMatchingContactsFromIndex(const QStandardItem *parent)
{
    Q_D(MergeDialog);
    // sum-up all children plus father from PersonsModel indexes in order to merge
    QList<QModelIndex> mergingList;
    int rows = parent->rowCount();

    for (int i = 0; i<rows; i++) {
        QStandardItem *child = parent->child(i,0);
        mergingList << d->personsModel->indexForUri(child->data(UriRole).toUrl());
    }
    mergingList.prepend(d->personsModel->indexForUri(parent->data(UriRole).toUrl()));
    // NOTE : not working so far, known issue, wait for the fix : let it comment 'til then
    //d->personsModel->createPersonFromIndexes(mergingList);
}

void MergeDialog::onMergeButtonClicked()
{
    QList<QStandardItem*> parents = checkedItems();
    Q_FOREACH (QStandardItem *parent , parents) {
        mergeMatchingContactsFromIndex(parent);
    }
    emit accept();
}

void MergeDialog::searchForDuplicatesFinished(KJob*)
{
    Q_D(MergeDialog);
    QList<Match> matches = d->duplicatesFinder->results();
    feedDuplicateModelFromMatches(d->duplicatesFinder->results());

    d->delegate = new MergeDelegate(d->view);
    d->view->setItemDelegate(d->delegate);

    // To extend the selected item
    connect(d->view->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            d->delegate, SLOT(onSelectedContactsChanged(QItemSelection,QItemSelection)));
    // To contract an already selected item
    connect(d->view, SIGNAL(doubleClicked(QModelIndex)),
            d->delegate, SLOT(onClickContactParent(QModelIndex)));
}

void MergeDialog::feedDuplicateModelFromMatches(const QList<Match> &matches)
{
    Q_D(MergeDialog);
    QMap<QPersistentModelIndex, QList<Match> > compareTable;
    QList<Match> currentValue;

    Q_FOREACH (const Match &match, matches) {
        currentValue = compareTable.value(match.indexA);

        if (currentValue.isEmpty()) { // new parent, create it
            QList<Match> firstList = QList<Match>() << match;
            compareTable.insert(match.indexA, firstList);
        } else { //know parent, add child
            currentValue.append(match);
            compareTable[match.indexA]= currentValue;
        }
    }
    // now build the model : 1st dimension = parent , 2nd dimension = children
    QStandardItem *rootItem = d->model->invisibleRootItem();
    QMap<QPersistentModelIndex, QList< Match > >::const_iterator i;

    for (i = compareTable.constBegin(); i != compareTable.constEnd(); ++i) {
        // Build the merge Contact in the model
        QStandardItem *parent = itemMergeContactFromMatch(i.key(), i->first());
        rootItem->appendRow(parent);

        Q_FOREACH (const Match &matchChild, compareTable.value(QModelIndex() , *i)) {
            QStandardItem *oneChild = itemMergeContactFromMatch(QModelIndex(), matchChild);
            parent->appendRow(oneChild);
        }
    }
}

QStandardItem* MergeDialog::itemMergeContactFromMatch(const QModelIndex &idx, const Match &match)
{
    Q_D(MergeDialog);
    QStandardItem *item = new QStandardItem;
    PersonDataPtr person(PersonData::createFromUri(idx.data(PersonsModel::UriRole).toUrl()));
    item->setData(person->uri(), UriRole);
    item->setCheckable(true);
    item->setCheckState(Qt::Checked);

    if (!idx.isValid()) { // child
        item->setData(qVariantFromValue<Match>(match), MergeReasonRole);
        item->setText(match.indexB.data(Qt::DisplayRole).toString());
    } else { // parent
        item->setText(match.indexA.data(Qt::DisplayRole).toString());
    }

    QVariant pix = idx.data(Qt::DecorationRole);
    if (!pix.isNull()) {
        item->setIcon(d->defaultPixmap);
    } else {
        item->setIcon(pix.value<QPixmap>());
    }
    return item;
}

QList<QStandardItem*> MergeDialog::checkedItems()
{
    Q_D(MergeDialog);
    QStandardItem *root = d->model->invisibleRootItem();
    int rows = root->rowCount();
    QList<QStandardItem*> results;
    for (int i = 0 ; i < rows ; i++) {
        QStandardItem *currentParent = root->child(i,0);
        if (currentParent->checkState() == Qt::Checked) {
            results.append(currentParent);
        }
    }
    return results;
}
