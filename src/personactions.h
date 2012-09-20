/*
    Persons Model
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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

#ifndef PERSONACTIONS_H
#define PERSONACTIONS_H

#include "kpeople_export.h"
#include <qabstractitemmodel.h>

class PersonData;
class QAction;
struct PersonActionsPrivate;

class KPEOPLE_EXPORT PersonActions : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(const QAbstractItemModel* peopleModel READ model WRITE setModel)
    Q_PROPERTY(int row READ row WRITE setRow)
    Q_PROPERTY(PersonData* person READ person WRITE setPerson)
    public:
        explicit PersonActions(QObject* parent = 0);
        virtual ~PersonActions();
        
        QList<QAction*> actions();
        void setModel(const QAbstractItemModel* model);
        const QAbstractItemModel* model() const;
        int row() const;
        void setRow(int row);
        
        PersonData* person() const;
        void setPerson(PersonData* data);
        
        Q_INVOKABLE void trigger(int actionsRow);
        
        virtual QVariant data(const QModelIndex& index, int role) const;
        virtual int rowCount(const QModelIndex& parent=QModelIndex()) const;

    private slots:
        void emailTriggered();
        void imTriggered();

    signals:
        void actionsChanged();

    private:
        void initialize(const QAbstractItemModel* model, int row);
        void initialize(const QModelIndex& personIndex);
        
        PersonActionsPrivate * const d_ptr;
        Q_DECLARE_PRIVATE(PersonActions);
};

#endif // PERSONACTIONS_H
