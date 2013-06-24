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


#include "implugin.h"

#include <QAction>

#include <KIcon>
#include <KLocalizedString>
#include <KPluginFactory>
#include <kdemacros.h>

#include <KTp/contact.h>
#include <KTp/actions.h>

#include <TelepathyQt/Account>
#include <TelepathyQt/ContactManager>

#include "impersonsdatasource.h"
#include "personpluginmanager.h"
#include "persondata.h"

enum IMActionType {
    TextChannel,
    AudioChannel,
    VideoChannel,
    FileTransfer,
    LogViewer
};

class IMAction : public QAction {
    Q_OBJECT
public:
    IMAction(const QString &text, const KIcon &icon, const KTp::ContactPtr &contact,
             const Tp::AccountPtr &account, IMActionType type, QObject *parent);
    KTp::ContactPtr contact() const;
    Tp::AccountPtr account() const;
    IMActionType type() const;
private:
    KTp::ContactPtr m_contact;
    Tp::AccountPtr m_account;
    IMActionType m_type;
};

IMAction::IMAction(const QString &text, const KIcon &icon, const KTp::ContactPtr &contact,
                   const Tp::AccountPtr &account, IMActionType type, QObject *parent):
    QAction(icon, text, parent),
    m_contact(contact),
    m_account(account),
    m_type(type)
{
}

KTp::ContactPtr IMAction::contact() const
{
    return m_contact;
}

Tp::AccountPtr IMAction::account() const
{
    return m_account;
}


IMActionType IMAction::type() const
{
    return m_type;
}

IMPlugin::IMPlugin(QObject* parent, const QVariantList &args):
AbstractPersonPlugin(parent)
{
}

QList<QAction*> IMPlugin::actionsForPerson(KPeople::PersonData *personData, QObject *parent)
{
    QList<QAction*> actions;

    IMPersonsDataSource *dataSource = dynamic_cast<IMPersonsDataSource*>(KPeople::PersonPluginManager::presencePlugin());
    if (!dataSource) {
        return actions;
    }

    QStringList imContactsIds = personData->imAccounts();

    for (int i=0;i<imContactsIds.size();i+=3) { //FIXME imAccounts() returns a silly datatype
        const QString contactId = imContactsIds[i+2];
        const KTp::ContactPtr contact = dataSource->contactForContactId(contactId);
        if (!contact || !contact->manager()) {
            continue;
        }
        const Tp::AccountPtr account = dataSource->accountForContact(contact);

        if (!account) {
            continue;
        }

        if (true) { //no such query for text chat capability, added an "if true" because makes the code look consistent
            QAction *action = new IMAction(i18n("Start Chat"),
                                KIcon("text-x-generic"),
                                contact,
                                account,
                                TextChannel,
                                parent);
            connect (action, SIGNAL(triggered(bool)), SLOT(onActionTriggered()));
            actions << action;
        }
        if (contact->audioCallCapability()) {
            QAction *action = new IMAction(i18n("Start Audio Call"),
                                KIcon("audio-headset"),
                                contact,
                                account,
                                AudioChannel,
                                parent);
            connect (action, SIGNAL(triggered(bool)), SLOT(onActionTriggered()));
            actions << action;
        }
        if (contact->videoCallCapability()) {
            QAction *action = new IMAction(i18n("Start Video Call"),
                                KIcon("camera-web"),
                                contact,
                                account,
                                VideoChannel,
                                parent);
            connect (action, SIGNAL(triggered(bool)), SLOT(onActionTriggered()));
            actions << action;
        }

        if (contact->fileTransferCapability()) {
            QAction *action = new IMAction(i18n("Send a file"),
                                        KIcon("mail-attachment"),
                                        contact,
                                        account,
                                        FileTransfer,
                                        parent);
            action->setDisabled(true); //FIXME: we need to prompt for file
            connect (action, SIGNAL(triggered(bool)), SLOT(onActionTriggered()));
            actions << action;
        }

        QAction *action = new IMAction(i18n("Open Log Viewer"),
                                    KIcon("documentation"),
                                    contact,
                                    account,
                                    LogViewer,
                                    parent);
        connect(action, SIGNAL(triggered(bool)), SLOT(onActionTriggered()));
        actions << action;
    }
    return actions;
}

void IMPlugin::onActionTriggered()
{
    IMAction *action = qobject_cast<IMAction*>(sender());
    KTp::ContactPtr contact = action->contact();
    Tp::AccountPtr account = action->account();
    IMActionType type = action->type();

    switch (type) {
        case TextChannel:
            KTp::Actions::startChat(account, contact);
            break;
        case AudioChannel:
            KTp::Actions::startAudioCall(account, contact);
            break;
        case VideoChannel:
            KTp::Actions::startAudioVideoCall(account, contact);
            break;
        case FileTransfer:
            //TODO: add filetransfer
            break;
        case LogViewer:
            KTp::Actions::openLogViewer(account, contact);
            break;
    }
}

#include "implugin.moc"
#include "moc_implugin.cpp"


K_PLUGIN_FACTORY( IMPluginFactory, registerPlugin<IMPlugin>(); )
K_EXPORT_PLUGIN( IMPluginFactory("ktp_kpeople_plugin") )
