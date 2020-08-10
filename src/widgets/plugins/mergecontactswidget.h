/*
    SPDX-FileCopyrightText: 2013 Franck Arrecot <franck.arrecot@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef MERGE_CONTACTS_WIDGET_H
#define MERGE_CONTACTS_WIDGET_H

#include "abstractpersondetailswidget.h"

#include "QPersistentModelIndex"

class QPushButton;
class PersonPresentationWidget;

namespace KPeople
{
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

    QList<QPersistentModelIndex> duplicateBusterFromPerson(const QUrl &uri) const;
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
    QList< QPair<QPersistentModelIndex, PersonPresentationWidget *> > m_listMergeContacts;
};

#endif // MERGE_CONTACTS_WIDGET_H
