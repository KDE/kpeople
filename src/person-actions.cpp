/*
    Copyright (C) 2013  Martin Klapetek <mklapetek@kde.org>

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


#include "person-actions.h"
#include "persons-model.h"

#include <QPersistentModelIndex>
#include <QAction>
#include <KIcon>
#include <KLocalizedString>
#include <KToolInvocation>

#include <KTp/contact.h>
#include <KTp/actions.h>
#include <KTp/types.h>
#include <TelepathyQt/Account>

struct PersonActionsPrivate {
    QList<QAction*> actions;

    QPersistentModelIndex index;
};

PersonActions::PersonActions(QObject *parent)
    : QAbstractListModel(parent),
    d_ptr(new PersonActionsPrivate)
{
}

PersonActions::~PersonActions()
{
    qDeleteAll(d_ptr->actions);
    delete d_ptr;
}

void PersonActions::setPerson(QAbstractItemModel* model, int row)
{
    setPerson(model->index(row, 0));
}

void PersonActions::setPerson(const QPersistentModelIndex& index)
{
    Q_D(PersonActions);
    beginResetModel();
    d->actions.clear();
    d->index = index;

    if (index.data(PersonsModel::ContactCanTextChatRole).toBool()) {
        QAction *textChatAction = new QAction(i18n("Start Chat"), this);
        textChatAction->setIcon(KIcon("text-x-generic"));

        connect(textChatAction, SIGNAL(triggered(bool)),
                this, SLOT(imChatTriggered()));

        d->actions.append(textChatAction);
    }

    if (index.data(PersonsModel::ContactCanAudioCallRole).toBool()) {
        QAction *audioCallAction = new QAction(i18n("Start Audio Call"), this);
        audioCallAction->setIcon(KIcon("audio-headset"));

        connect(audioCallAction, SIGNAL(triggered(bool)),
                this, SLOT(imAudioCallTriggered()));

        d->actions.append(audioCallAction);
    }

    if (index.data(PersonsModel::ContactCanVideoCallRole).toBool()) {
        QAction *videoCallAction = new QAction(i18n("Start Video Call"), this);
        videoCallAction->setIcon(KIcon("camera-web"));

        connect(videoCallAction, SIGNAL(triggered(bool)),
                this, SLOT(imVideoCallTriggered()));

        d->actions.append(videoCallAction);
    }

    if (!index.data(PersonsModel::EmailRole).isNull()) {
        QString email = index.data(PersonsModel::EmailRole).toString();
        QAction *emailAction = new QAction(i18n("Send Email to %1", email), this);
        emailAction->setIcon(KIcon(""));

        connect(emailAction, SIGNAL(triggered(bool)),
                this, SLOT(emailTriggered()));

        d->actions.append(emailAction);
    }
    endResetModel();
    emit personChanged();
}

QVariant PersonActions::data(const QModelIndex &index, int role) const
{
    Q_D(const PersonActions);

    if (!index.isValid()) {
        return QVariant();
    }

    switch (role) {
        case Qt::DisplayRole:
            return d->actions[index.row()]->text();
        case Qt::DecorationRole:
            return d->actions[index.row()]->icon();
        case Qt::ToolTip:
            return d->actions[index.row()]->toolTip();
    }

    return QVariant();
}

int PersonActions::rowCount(const QModelIndex& parent) const
{
    Q_D(const PersonActions);

    return parent.isValid() ? 0 : d->actions.size();
}

void PersonActions::imChatTriggered() const
{
    Q_D(const PersonActions);

    KTp::Actions::startChat(d->index.data(PersonsModel::IMAccountRole).value<Tp::AccountPtr>(),
                            d->index.data(PersonsModel::IMContactRole).value<KTp::ContactPtr>());
}

void PersonActions::imAudioCallTriggered() const
{
    Q_D(const PersonActions);
    KTp::Actions::startAudioCall(d->index.data(PersonsModel::IMAccountRole).value<Tp::AccountPtr>(),
                                 d->index.data(PersonsModel::IMContactRole).value<KTp::ContactPtr>());
}

void PersonActions::imVideoCallTriggered() const
{
    Q_D(const PersonActions);
    KTp::Actions::startAudioVideoCall(d->index.data(PersonsModel::IMAccountRole).value<Tp::AccountPtr>(),
                                      d->index.data(PersonsModel::IMContactRole).value<KTp::ContactPtr>());
}

void PersonActions::emailTriggered() const
{
    Q_D(const PersonActions);
    KToolInvocation::invokeMailer(d->index.data(PersonsModel::EmailRole).toString(), QString());
}

void PersonActions::triggerAction(int row) const
{
    Q_D(const PersonActions);
    d->actions[row]->trigger();
}

QList< QAction* > PersonActions::actions() const
{
    Q_D(const PersonActions);
    return d->actions;
}
