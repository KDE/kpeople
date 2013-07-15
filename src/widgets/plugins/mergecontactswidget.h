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


#ifndef MERGE_CONTACT_WIDGET_H
#define MERGE_CONTACT_WIDGET_H

#include "abstractpersondetailswidget.h"

#include "QPersistentModelIndex"

class QPushButton;
class PersonPresentationWidget;

namespace KPeople {
class DuplicatesFinder;
}

class MergeContactsWidget : public KPeople::AbstractPersonDetailsWidget
{
    Q_OBJECT
public:
    explicit MergeContactsWidget(QWidget *parent, const QVariantList &args);
    void setPerson(KPeople::PersonData *person);
    void setPersonsModel(KPeople::PersonsModel *model);

    void searchForDuplicates();
    void fillDuplicatesWidget(const QList<QPersistentModelIndex> &duplicates);

    QList<QPersistentModelIndex> duplicateBusterFromPerson(const QUrl& uri) const;
    QList<QPersistentModelIndex> getContactsCheckedToMerge() const;

private Q_SLOTS:
    void onMergeButtonPressed();
    void onMergePossibilitiesButtonPressed();
    void searchForDuplicatesFinished();
private:
    KPeople::PersonData *m_person;
    KPeople::PersonsModel *m_model;
    QPushButton *m_mergeButton;

    QWidget *m_containerListDetails;
    KPeople::DuplicatesFinder *m_duplicatesBuster;
    QList< QPair<QPersistentModelIndex, PersonPresentationWidget*> > m_listMergeContacts;
};


#endif // MERGE_CONTACT_WIDGET_H
