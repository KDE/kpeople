/*
    Copyright (C) 2013  David Edmundson <davidedmundson@kde.org>

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



#ifndef EMAIL_DETAILS_WIDGET_H
#define EMAIL_DETAILS_WIDGET_H

#include "abstractfieldwidgetfactory.h"

using namespace KPeople;

class EmailFieldsPlugin : public AbstractFieldWidgetFactory
{
public:
    virtual QString label() const;
    virtual int sortWeight() const;
    virtual QWidget* createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList &contacts, QWidget* parent) const;
};

#endif // EMAIL_DETAILS_WIDGET_H
