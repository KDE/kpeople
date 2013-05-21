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


#include "datagenerator.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>

#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NFO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NMM>

#include <KUrl>
#include <KDebug>

#include <QtCore/QFile>
#include <QtCore/QDateTime>
#include <QtCore/QUuid>
#include <QtCore/QDir>

#include <soprano/vocabulary.h>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

namespace Nepomuk2 {
namespace Test {

DataGenerator::DataGenerator()
{
    qsrand( QDateTime::currentMSecsSinceEpoch() );
    m_dir.setAutoRemove( false );
}

DataGenerator::~DataGenerator()
{
}


namespace {
    QUrl generateFileUrl( const QString& dir ) {
        QUrl fileUrl;
        while( 1 ) {
            int r = qrand();
            fileUrl = QUrl::fromLocalFile( dir + "/" + QString::number(r) );

            if( !QFile::exists(fileUrl.toLocalFile()) )
                return fileUrl;
        }
    }
}

QUrl DataGenerator::createPlainTextFile(const QString& content)
{
    SimpleResourceGraph graph = createPlainTextFile( generateFileUrl( m_dir.name() ), content );
    StoreResourcesJob* job = graph.save();
    job->exec();
    if( job->error() )
        return QUrl();

    SimpleResource res = graph.toList().first();
    return job->mappings().value( res.uri() );
}

QUrl DataGenerator::createTag(const QString& identifier)
{
    SimpleResource res;
    res.addType( NAO::Tag() );
    res.setProperty( NAO::identifier(), identifier);

    SimpleResourceGraph graph;
    graph << res;

    StoreResourcesJob* job = graph.save();
    job->exec();
    if( job->error() )
        return QUrl();

    return job->mappings().value( res.uri() );
}

QUrl DataGenerator::createMusicFile(const QString& title, const QString& artistName, const QString& albumName)
{
    SimpleResourceGraph graph = createMusicFile( generateFileUrl( m_dir.name() ), title,
                                                 artistName, albumName );
    StoreResourcesJob* job = graph.save();
    job->exec();
    if( job->error() )
        return QUrl();

    QUrl resUri;
    QList<SimpleResource> list = graph.toList();
    foreach( const SimpleResource& res, list ) {
        if( res.contains( RDF::type(), NFO::FileDataObject() ) ) {
            resUri = res.uri();
            break;
        }
    }

    return job->mappings().value( resUri );
}



SimpleResourceGraph DataGenerator::generateGraph(int n)
{
    // Fow now, doing it all in one go
    SimpleResourceGraph graph;
    for( int i=0; i<n; i++) {
        QUrl fileUrl = generateFileUrl( m_dir.name() );

        int n = qrand() % 2;
        switch( n ) {
            case 0:
                graph += createPlainTextFile(fileUrl, generateName());
                break;
            case 1:
                graph += createMusicFile(fileUrl, generateName(), generateName(), generateName() );
                break;
            case 2:
                graph += createImageFile(fileUrl);
        }
    }

    return graph;
}

bool DataGenerator::generate( int n )
{
    StoreResourcesJob* job = generateGraph( n ).save();
    job->exec();
    if( job->error() ) {
        kDebug() << job->error();
        return false;
    }

    return true;
}

QString DataGenerator::generateName()
{
    // FIXME: Use something better
    return QUuid::createUuid().toString();
}

SimpleResourceGraph DataGenerator::createPlainTextFile(const QUrl& url, const QString& content)
{
    QFile file( url.toLocalFile() );
    file.open( QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text );

    // Does this really matter?
    QTextStream stream(&file);
    stream << content;

    file.close();

    SimpleResource fileRes;
    fileRes.addType( NFO::TextDocument() );
    fileRes.addType( NFO::FileDataObject() );
    fileRes.addProperty( NIE::plainTextContent(), content );
    fileRes.addProperty( NIE::mimeType(), QLatin1String("text/plain") );
    fileRes.addProperty( NIE::url(), url );
    fileRes.addProperty( NFO::fileName(), KUrl(url).fileName() );

    return SimpleResourceGraph() << fileRes;
}

SimpleResourceGraph DataGenerator::createFolder(const QUrl& url)
{
    QDir().mkdir( url.toLocalFile() );

    SimpleResource folder;
    folder.addType( NFO::Folder() );
    folder.addType( NFO::FileDataObject() );
    folder.addProperty( NIE::url(), url );
    folder.addProperty( NFO::fileName(), KUrl(url).fileName() );

    return SimpleResourceGraph() << folder;
}


namespace {
    void createFile(const QUrl& url) {
        QFile file( url.toLocalFile() );
        file.open( QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text );
        file.close();
    }
}
SimpleResourceGraph DataGenerator::createMusicFile(const QUrl& url, const QString& title,
                                                   const QString& artistName, const QString& albumName)
{
    createFile( url );

    SimpleResource artistRes;
    artistRes.addType( NCO::Contact() );
    artistRes.setProperty( NCO::fullname(), artistName );

    SimpleResource albumRes;
    albumRes.addType( NMM::MusicAlbum() );
    albumRes.setProperty( NIE::title(), albumName );

    SimpleResource fileRes;
    fileRes.addType( NMM::MusicPiece() );
    fileRes.addType( NFO::FileDataObject() );
    fileRes.addProperty( NIE::title(), title );
    fileRes.addProperty( NIE::url(), url );
    fileRes.addProperty( NFO::fileName(), KUrl(url).fileName() );
    fileRes.addProperty( NMM::performer(), artistRes );
    fileRes.addProperty( NMM::musicAlbum(), albumRes );

    SimpleResourceGraph graph;
    graph << artistRes << albumRes << fileRes;

    return graph;
}

SimpleResourceGraph DataGenerator::createImageFile(const QUrl& url)
{
    createFile( url );

    return SimpleResourceGraph();
}

}
}
