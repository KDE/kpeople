/*
    Copyright (C) 2013  Franck Arrecot <franck.arrecot@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "personpresentationwidget.h"

#include "QHBoxLayout";
#include "QLabel"

PersonPresentationWidget::PersonPresentationWidget(const QString &name, const QPixmap &avatar, QWidget *parent):
    QWidget(parent)
{
    setLayout(new QHBoxLayout(this));

    m_email = new QLabel(this);
    m_phone = new QLabel(this);
    m_name = new QLabel(this);
    m_pixmap = new QLabel(this);

    m_selected = new QCheckBox(this);
    m_selected->setChecked(true);

    m_name->setText(name);
    m_pixmap->setPixmap(avatar.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    draw() ;
}

PersonPresentationWidget::~PersonPresentationWidget()
{
}

void PersonPresentationWidget::draw()
{
    layout()->addWidget(m_pixmap);
    layout()->addWidget(m_name);
    layout()->addWidget(m_selected);
}