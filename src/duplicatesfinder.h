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

#ifndef DUPLICATESFINDER_H
#define DUPLICATESFINDER_H

#include <KJob>
#include <QVariantList>
#include "kpeople_export.h"

struct Match {
    Match(int r, int a, int b) : role(r), rowA(a), rowB(b) {}
    
    int role;
    int rowA, rowB;
};

class PersonsModel;
class KPEOPLE_EXPORT DuplicatesFinder : public KJob
{
    Q_OBJECT
    public:
        explicit DuplicatesFinder(PersonsModel* model, QObject* parent = 0);
        
        virtual void start();

    private:
        int matchAt(const QVariant& value, const QVector< QVariantList >& collectedValues, int role) const;
        PersonsModel* m_model;
};

#endif // DUPLICATESFINDER_H
