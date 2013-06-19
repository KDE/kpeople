/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2013  David <D.Edmundson@lboro.ac.uk>

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


#include "logmodel.h"
#include <persondata.h>
#include <QFile>
#include <QStringList>
#include <QIcon>
#include <QDebug>

class LogEntry
{
public:
    QString hash;
    QString contactName;
    QString contactEmail;
    QString commitDate;
    QString commitMessage;
};

LogModel::LogModel(QObject* parent): QAbstractTableModel(parent)
{
    QFile logFile("/home/david/log");
    logFile.open(QIODevice::ReadOnly);

    while(!logFile.atEnd()) {
        QList<QByteArray> parts = logFile.readLine().split(',');
        if (parts.size() ==5) {
            LogEntry entry;
            entry.hash =  parts[0];
            entry.contactName = parts[1];
            entry.contactEmail = parts[2];
            entry.commitDate = parts[3];
            entry.commitMessage = parts[4];

            const QString &emailAddress = parts[2];
            if (!m_avatarCache.contains(emailAddress)) {
                PersonData data;
                data.setContactId(emailAddress);
                m_avatarCache[emailAddress] = data.avatar().toLocalFile();
                qDebug() << "adding " << emailAddress;
            }

            m_entries.append(entry);
        }
    }

}

QVariant LogModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0:
                return m_entries[row].hash;
            case 1:
                return m_entries[row].contactName;
            case 2:
                return m_entries[row].contactEmail;
            case 3:
                return m_entries[row].commitMessage;
            case 4:
                return m_entries[row].commitDate;
        }
    }
    if (role == Qt::DecorationRole && index.column() == 1) {
        return QIcon(m_avatarCache[m_entries[row].contactEmail]);
    }

    return QVariant();
}

int LogModel::columnCount(const QModelIndex& parent) const
{
    return 5;
}

int LogModel::rowCount(const QModelIndex& parent) const
{
    return m_entries.size();
}

