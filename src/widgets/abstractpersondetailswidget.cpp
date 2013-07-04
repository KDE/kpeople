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


#include "abstractpersondetailswidget.h"

namespace KPeople {
class AbstractPersonDetailsWidgetPrivate
{
public:
    QString title;
    QIcon icon;
    bool active;
};
}

using namespace KPeople;

AbstractPersonDetailsWidget::AbstractPersonDetailsWidget(QWidget *parent) :
    QWidget(parent),
    d_ptr(new AbstractPersonDetailsWidgetPrivate())
{
    Q_D(AbstractPersonDetailsWidget);
    d->active = true;
}

AbstractPersonDetailsWidget::~AbstractPersonDetailsWidget()
{
    delete d_ptr;
}

void AbstractPersonDetailsWidget::setPersonsModel(PersonsModel *model)
{
    Q_UNUSED(model);
}

void AbstractPersonDetailsWidget::setIcon(const QIcon &icon)
{
    Q_D(AbstractPersonDetailsWidget);
    d->icon = icon;
}

QIcon AbstractPersonDetailsWidget::icon() const
{
    Q_D(const AbstractPersonDetailsWidget);
    return d->icon;
}

void AbstractPersonDetailsWidget::setTitle(const QString &title)
{
    Q_D(AbstractPersonDetailsWidget);
    d->title = title;
}

QString AbstractPersonDetailsWidget::title() const
{
    Q_D(const AbstractPersonDetailsWidget);
    return d->title;
}

void AbstractPersonDetailsWidget::setActive(bool active)
{
    Q_D(AbstractPersonDetailsWidget);
    d->active = active;
    emit activeChanged(active);
}

bool AbstractPersonDetailsWidget::active() const
{
    Q_D(const AbstractPersonDetailsWidget);
    return d->active;
}
