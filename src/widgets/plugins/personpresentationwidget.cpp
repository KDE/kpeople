/*
    SPDX-FileCopyrightText: 2013 Franck Arrecot <franck.arrecot@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
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
