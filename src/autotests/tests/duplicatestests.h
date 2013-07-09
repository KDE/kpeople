/*
    Duplicates Finder Auto Test
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


#ifndef DUPLICATESTESTS_H
#define DUPLICATESTESTS_H

#include <QtCore/QObject>
#include <QUrl>
#include "../lib/testbase.h"

class DuplicatesTests : public Nepomuk2::TestBase
{
    Q_OBJECT
private Q_SLOTS:
    void init();
    void duplicatesSpecificPersonSearch();

private:
    void initContact1();
    void initPersonA();
    void initPersonB();

    QUrl m_contact1Uri;
    QUrl m_contact2Uri;
    QUrl m_contact2BISUri;
    QUrl m_contact3Uri;
    QUrl m_personAUri;
    QUrl m_personBUri;
};

#endif // DUPLICATESTESTS_H
