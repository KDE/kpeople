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

#ifndef NOTE_H
#define NOTE_H

#include "abstractfieldwidgetfactory.h"
#include "plugins/emaillistmodel.h"
#include <QItemSelection>
#include <KJob>
#include <Akonadi/Collection>
#include <KABC/Addressee>
#include <KABC/AddresseeList>
#include <Akonadi/ItemModifyJob>
#include <KDateTime>
#include <QTextEdit>
#include <QPushButton>

using namespace KPeople;


class Note : public AbstractFieldWidgetFactory
{
    Q_OBJECT
public:
    explicit Note(QObject* parent = 0);
    virtual QString label() const;
    virtual int sortWeight() const;
    virtual QWidget* createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList& contacts, QWidget* parent) const;

private Q_SLOTS:
    void saveNote(bool);
    void textChanged();
    void contactModifyResult(KJob*);

private:
    QTextEdit* m_noteEditor;
    QPushButton* m_saveBtn;
    KABC::Addressee m_person;

};

#endif // NOTE_H
