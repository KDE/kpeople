/*
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef ABSTRATPERSONDETAILSWIDGET_H
#define ABSTRATPERSONDETAILSWIDGET_H

#include <QWidget>

#include <KIcon>

namespace KPeople
{
class PersonData;
class PersonsModel;
class AbstractPersonDetailsWidgetPrivate;

class AbstractPersonDetailsWidget : public QWidget
{
Q_OBJECT
public:
    explicit AbstractPersonDetailsWidget(QWidget *parent = 0);
    virtual ~AbstractPersonDetailsWidget();

    /**Update to show the latest information for this person*/
    virtual void setPerson(PersonData *personData) = 0;
    virtual void setPersonsModel(PersonsModel *model) ;

    /** The title to be displayed in a group header*/
    QString title() const;
    /** The icon to be shown in the group header*/
    KIcon icon() const;

    bool active() const;

signals:
    void activeChanged(bool);

protected:
    void setTitle(const QString &title);
    void setIcon(const KIcon &icon);
    /** Sets whether this widget contains any useful information.
     * If not it along with any group headers will not be shown
     */
    void setActive(bool active);

private:
    AbstractPersonDetailsWidgetPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(AbstractPersonDetailsWidget);
};
}

#endif // ABSTRATPERSONDETAILSWIDGET_H
