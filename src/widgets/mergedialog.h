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

#include <kpeople/widgets/kpeoplewidgets_export.h>

#include <QDialog>
#include <QModelIndex>

class QStandardItem;
class KJob;
class MergeDialogPrivate;

namespace KPeople
{
class PersonsModel;
class Match;

/**
 * The merge dialog will be used to provide a GUI to attempt to
 * figure out what contacts should be merged.
 *
 * By properly mergeing contacts, the user will end up with having more
 * information on each person.
 */
class KPEOPLEWIDGETS_EXPORT MergeDialog : public QDialog
{
    Q_OBJECT

public:
    enum Role {
        NameRole = Qt::DisplayRole,
        UriRole = Qt::UserRole + 1,
        PixmapRole,
        MergeReasonRole
    };

    explicit MergeDialog(QWidget *parent = nullptr);
    ~MergeDialog();

    /**
     * Specifies which PersonsModel will be used to look for duplicates.
     */
    void setPersonsModel(PersonsModel *model);

private Q_SLOTS:
    void searchForDuplicates();
    void searchForDuplicatesFinished(KJob *);
    void onMergeButtonClicked();

private:
    QStandardItem *itemMergeContactFromMatch(bool parent, const KPeople::Match &match);
    void feedDuplicateModelFromMatches(const QList<Match> &matches);

    MergeDialogPrivate *const d_ptr;
    Q_DECLARE_PRIVATE(MergeDialog)
};

}

#endif // MERGEDIALOG_H
