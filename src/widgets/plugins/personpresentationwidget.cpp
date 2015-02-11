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

#include "personpresentationwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSize>

PersonPresentationWidget::PersonPresentationWidget(const QString &name, const QIcon &avatar, QWidget *parent)
    : QWidget(parent)
{
    setLayout(new QHBoxLayout(this));

    m_email = new QLabel(this);
    m_phone = new QLabel(this);
    m_name = new QLabel(this);
    m_pixmap = new QLabel(this);

    m_selected = new QCheckBox(this);
    m_selected->setChecked(true);

    m_name->setText(name);

    // Scaling the pixmap size to the name font
    int pixmapHeight = m_name->fontMetrics().height() * 3;
    QSize pixSize(pixmapHeight, pixmapHeight);

    m_pixmap->setPixmap(avatar.pixmap(pixSize)
                        .scaled(pixSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
