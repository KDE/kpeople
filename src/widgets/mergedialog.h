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

#ifndef MERGEDIALOG_H
#define MERGEDIALOG_H

#include "kpeople_export.h"

#include <QDialog>
#include <QModelIndex>

class QStandardItem;
class KJob;
class MergeDialogPrivate;

namespace KPeople {
class PersonsModel;
class Match;

class KPEOPLE_EXPORT MergeDialog : public QDialog
{
    Q_OBJECT

public:
    enum Role {
        NameRole = Qt::DisplayRole,
        UriRole = Qt::UserRole + 1,
        PixmapRole,
        MergeReasonRole
    };

    explicit MergeDialog(QWidget *parent = 0);
    ~MergeDialog();
    void setPersonsModel(PersonsModel *model);

private:
    QList<QStandardItem*> checkedItems();
    void mergeMatchingContactsFromIndex(const QStandardItem *parent);
    QStandardItem* itemMergeContactFromMatch(const QModelIndex &idx, const Match &match);

    void feedDuplicateModelFromMatches(const QList<Match> &matches) ;

private Q_SLOTS:
    void searchForDuplicates();
    void searchForDuplicatesFinished(KJob *);
    void onMergeButtonClicked();

private:
    MergeDialogPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(MergeDialog)
};

}

#endif // MERGEDIALOG_H
