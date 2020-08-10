/*
    SPDX-FileCopyrightText: 2013 Franck Arrecot <franck.arrecot@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
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
    PersonPresentationWidget(const QString &name, const QIcon &avatar, QWidget *parent = nullptr);
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
