/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2012  Vishesh Handa <me@vhanda.in>

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


#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>

#include <KTempDir>
#include <QtCore/QUrl>

namespace Nepomuk2 {
namespace Test {

class DataGenerator
{
public:
    DataGenerator();
    virtual ~DataGenerator();

    SimpleResourceGraph generateGraph( int n );
    bool generate( int n );

    static SimpleResourceGraph createPlainTextFile(const QUrl& url, const QString& content);
    static SimpleResourceGraph createFolder(const QUrl& url);
    static SimpleResourceGraph createMusicFile(const QUrl& url, const QString& title,
                                               const QString& artistName, const QString& albumName);
    static SimpleResourceGraph createImageFile(const QUrl& url);

    QUrl createTag(const QString& identifier );
    QUrl createPlainTextFile( const QString& content );
    QUrl createMusicFile(const QString& title, const QString& artistName, const QString& albumName);

private:
    int m_numFiles;
    KTempDir m_dir;

    QString generateName();
};

}
}

#endif // DATAGENERATOR_H
