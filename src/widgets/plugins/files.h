/*
 * Copyright 2014  Nilesh Suthar <nileshsuthar@live.in>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef FILES_H
#define FILES_H
#define JOB_PROPERTY "_KGAPI2Job"

#include "abstractfieldwidgetfactory.h"
#include <QItemSelection>
#include <KDateTime>
#include <KConfig>
#include <KConfigGroup>
#include <QListView>
#include <QStandardItemModel>
#include <LibKGAPI2/Account>
#include <LibKGAPI2/Job>

using namespace KPeople;

class Files : public AbstractFieldWidgetFactory
{
    Q_OBJECT
public:
    explicit Files(QObject* parent = 0);
    virtual QString label() const;
    virtual int sortWeight() const;
    virtual QWidget* createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList& contacts, QWidget* parent) const;

private slots:
    void authenticate();
    void slotAuthJobFinished(KGAPI2::Job*);
    void slotFileFetchJobFinished(KGAPI2::Job*);
    void getFiles();
    void updateAccountToken(const KGAPI2::AccountPtr&, KGAPI2::Job *restartJob = 0);
    void viewBtnClick(bool);
    void editBtnClick(bool);

private:
    KABC::Addressee m_person;
    QListView* m_ListView;
    QStandardItemModel* m_model;
    KGAPI2::AccountPtr m_account;
};
#endif // FILES_H
