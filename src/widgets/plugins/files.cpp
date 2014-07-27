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

#include "files.h"

#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QStandardItem>

#include <KLocale>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KLocalizedString>
#include <KABC/Addressee>

Files::Files(QObject* parent): AbstractFieldWidgetFactory(parent)
{
}

QWidget* Files::createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList& contacts, QWidget* parent) const
{
    Q_UNUSED(contacts);
    QWidget* widget = new QWidget(parent);

    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(widget);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFixedHeight(widget->height());

    const_cast<Files*>(this)->m_person = person;
    const_cast<Files*>(this)->m_ListView = new QListView();
    const_cast<Files*>(this)->m_model = new QStandardItemModel();

    m_ListView->setModel(m_model);
    m_ListView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_ListView->show();

    layout->addWidget(m_ListView);
    widget->setLayout(layout);
    return scrollArea;
}

QString Files::label() const
{
    return i18n("Files");
}

int Files::sortWeight() const
{
    return 0;
}

#include "files.moc"
