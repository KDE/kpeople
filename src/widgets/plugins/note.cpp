/*
 * Copyright 2014  Nilesh Suthar <nileshsuthar@live.in>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "note.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <KPluginFactory>
#include <KLocalizedString>

#include <KPluginLoader>

#include <QDebug>

using namespace Akonadi;


Note::Note(QObject* parent): AbstractFieldWidgetFactory(parent)
{
}

QWidget* Note::createDetailsWidget(const KABC::Addressee& person, const KABC::AddresseeList &contacts, QWidget* parent) const
{
    Q_UNUSED(contacts);
    QWidget *widget = new QWidget(parent);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0,0,0,0);

    const_cast<Note*> (this)->m_noteEditor = new QTextEdit(parent);
    const_cast<Note*> (this)->m_saveBtn = new QPushButton("Save");
    const_cast<Note*> (this)->m_person = person;

    m_noteEditor->setText(person.note());

//     if(person.note() != "")
//         m_noteEditor->setDisabled(true);
    
    connect(m_saveBtn,SIGNAL(clicked(bool)),this,SLOT(saveNote(bool)));
    connect(m_noteEditor,SIGNAL(textChanged()),this,SLOT(textChanged()));
    this->m_saveBtn->setDisabled(true);
    layout->addWidget(m_noteEditor);
    layout->addWidget(m_saveBtn);
    widget->setLayout(layout);
    return widget;
}

void Note::saveNote(bool)
{
    QString newNote = this->m_noteEditor->toPlainText();
    
    //FIXME Some ids are ktp:// in form so they are not being saved
    const KUrl &url = KUrl(m_person.custom("akonadi","id"));
    Akonadi::Item item = Item::fromUrl(url);
    m_person.setNote(newNote);
    item.setPayload<KABC::Addressee>( m_person );
    item.setMimeType( KABC::Addressee::mimeType() );
    Akonadi::ItemModifyJob *job = new Akonadi::ItemModifyJob( item );
    connect( job, SIGNAL( result( KJob* ) ), SLOT( contactModifyResult( KJob* ) ) );

}
void Note::contactModifyResult(KJob* job)
{
    if ( job->error() != 0 ) {
        // error handling, see job->errorString()
        return;
    }
}


void Note::textChanged()
{
    this->m_saveBtn->setDisabled(false);
}

QString Note::label() const
{
    return i18n("Notes");
}

int Note::sortWeight() const
{
    return 0;
}

#include "note.moc"

