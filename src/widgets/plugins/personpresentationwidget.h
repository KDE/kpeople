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

#ifndef PERSONPRESENTATIONWIDGET_H
#define PERSONPRESENTATIONWIDGET_H

#include <QWidget>
#include <QCheckBox>

class QLabel;

class PersonPresentationWidget : public QWidget
{
    Q_OBJECT
public:
    PersonPresentationWidget(const QString &name, const QIcon &avatar, QWidget *parent = 0);
    ~PersonPresentationWidget();

    bool isContactSelected() const
    {
        return m_selected->isChecked() ;
    }
private:
    void draw();
    QLabel *m_pixmap;
    QLabel *m_phone;
    QLabel *m_email;
    QLabel *m_name;
    QCheckBox *m_selected;
};

#endif // PERSONPRESENTATIONWIDGET_H
