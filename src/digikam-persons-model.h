/*
    Digikam persons model
    Copyright (C) 2012  Martin Klapetek <martin.klapetek@gmail.com>

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


#ifndef DIGIKAM_PERSONS_MODEL_H
#define DIGIKAM_PERSONS_MODEL_H

#include "kpeople_export.h"

#include <QModelIndex>
#include <QUrl>
#include <QPixmap>

class DigikamPersonCacheItemFacet;

class KPEOPLE_EXPORT DigikamPersonsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(Role)

public:

    enum Role {
        // general roles
        ContactIdRole = Qt::UserRole,
        AccountIdRole,
        ConnectionStatusTypeRole,
        PresenceIconRole
    };

    DigikamPersonsModel(QHash<QUrl, DigikamPersonCacheItemFacet*> *data, QObject *parent = 0);
    virtual ~DigikamPersonsModel();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QUrl queryPersonByName(const QString &name);
    QPixmap presencePixmapForUri(const QUrl &uri);

    ///Called from Manager, so that the model can be properly updated
    void beginInsertData(const QModelIndex &index, int first, int last);
    void endInsertData();

private:
    QHash<QUrl, DigikamPersonCacheItemFacet*> *m_data;
};

#endif // DIGIKAM_PERSONS_MODEL_H
