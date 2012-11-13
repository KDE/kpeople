/*
    Persons Model
    Copyright (C) 2012  Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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

#include "personactions.h"
#include "persons-model.h"
#include "persondata.h"
#include "nepomuk-tp-channel-delegate.h"

#include <KLocalizedString>

#include <QAbstractItemModel>
#include <QAction>

#include <KToolInvocation>
#include <KDebug>

#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/Resource>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Variant>
#include <Soprano/Node>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

Q_DECLARE_METATYPE(QModelIndex);

struct PersonActionsPrivate {
    PersonActionsPrivate() : row(-1), model(0), person(0) {}
    ~PersonActionsPrivate() {}

    int row;
    const QAbstractItemModel *model;
    QList<QAction*> actions;
    PersonData *person;

    QAction* createEmailAction(PersonActions* pactions, const QIcon& icon, const QString& name, const QString& email)
    {
        QAction* action = new QAction(icon, i18n("Send e-mail to '%1'", name), pactions);
        action->setProperty("email", email);
        QObject::connect(action, SIGNAL(triggered(bool)), pactions, SLOT(emailTriggered()));
        return action;
    }

    QList<QAction*> createIMActions(PersonActions* pactions, const QUrl& contactUri, const QString& imrole, const QString& nickname)
    {
        Q_ASSERT(!contactUri.isEmpty());
        QList<QAction*> actions;
        const QString query = QString::fromLatin1("select ?cap WHERE { "
                    "%1                         nco:hasIMAccount            ?nco_hasIMAccount. "
                    "?nco_hasIMAccount          nco:hasIMCapability         ?cap. "
                "   }").arg( Soprano::Node::resourceToN3(contactUri) );
        Soprano::Model* m = Nepomuk2::ResourceManager::instance()->mainModel();
        Soprano::QueryResultIterator it = m->executeQuery(query, Soprano::Query::QueryLanguageSparql);
        while(it.next()) {
            QString ss = it["cap"].toString();

            QAction *action = new QAction(pactions);
            action->setProperty("uri", contactUri);
            action->setProperty("imrole", imrole);
            action->setProperty("capability", ss);
            action->setText(i18n("%1 with '%2'", ss.mid(ss.lastIndexOf('#')+13), nickname));
            QObject::connect(action, SIGNAL(triggered(bool)), pactions, SLOT(imTriggered()));
            actions += action;
        }
        return actions;
    }
};

PersonActions::PersonActions(QObject* parent)
    : QAbstractListModel(parent)
    , d_ptr(new PersonActionsPrivate)
{}

PersonActions::~PersonActions()
{
    delete d_ptr;
}

QList< QAction* > PersonActions::actions()
{
    Q_D(const PersonActions);
    return d->actions;
}

const QAbstractItemModel* PersonActions::model() const
{
    Q_D(const PersonActions);
    return d->model;
}

void PersonActions::setModel(const QAbstractItemModel* model)
{
    Q_D(PersonActions);
    d->model = model;
    initialize(d->model, d->row);
}

int PersonActions::row() const
{
    Q_D(const PersonActions);
    return d->row;
}

void PersonActions::setRow(int row)
{
    Q_D(PersonActions);
    d->row = row;
    initialize(d->model, d->row);
}

void PersonActions::initialize(const QAbstractItemModel* model, int row)
{
    if(!model || row<0)
        return;

    Q_D(PersonActions);
    QModelIndex idx = model->index(row, 0);
    d->model = idx.model();
    d->row = idx.row();

    initialize(idx);
}

void PersonActions::initialize(const QModelIndex& idx)
{
    Q_D(PersonActions);
    Q_ASSERT(idx.isValid());

    d->model = idx.model();
    d->row = idx.row();

    int rows = d->model->rowCount(idx);
    beginResetModel();
    qDeleteAll(d->actions);
    d->actions.clear();
    for(int i=0; i<rows; i++) {
        QModelIndex idxContact = idx.child(i, 0);
        switch(idxContact.data(PersonsModel::ContactTypeRole).toInt()) {
            case PersonsModel::Email:
                d->actions += d->createEmailAction(this, idxContact.data(Qt::DecorationRole).value<QIcon>(), idxContact.data().toString(), idxContact.data(PersonsModel::EmailRole).toString());
                break;
            case PersonsModel::IM:
                d->actions += d->createIMActions(this, idxContact.data(PersonsModel::UriRole).toUrl(), idxContact.data(PersonsModel::IMRole).toString(), idxContact.data().toString());
                break;
            case PersonsModel::Phone:
            case PersonsModel::MobilePhone:
            case PersonsModel::Postal:
                break;
        }
    }
    endResetModel();
    emit actionsChanged();
}

void PersonActions::emailTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    KToolInvocation::invokeMailer(action->property("email").toString(), QString());
}

void PersonActions::imTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    const QString query = QString::fromLatin1("select ?telepathy_accountIdentifier WHERE {"
        "%1                         nco:hasIMAccount            ?nco_hasIMAccount . "
        "?nco_hasIMAccount          nco:isAccessedBy            ?nco_isAccessedBy . "
        "?nco_isAccessedBy          telepathy:accountIdentifier ?telepathy_accountIdentifier . "
    "   }").arg( Soprano::Node::resourceToN3(action->property("uri").toUrl()) );

    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator qit = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    QString account;
    if( qit.next() ) {
        account = qit["telepathy_accountIdentifier"].toString();
    }

    if(!account.isEmpty()) {
        Q_D(const PersonActions);
        qobject_cast<const PersonsModel*>(d->model)->tpChannelDelegate()->startIM(account,
                                                                                  action->property("imrole").toString(),
                                                                                  action->property("capability").toString());
    }
}

QVariant PersonActions::data(const QModelIndex& index, int role) const
{
    Q_D(const PersonActions);
    int row = index.row();
    if(index.isValid() && row<d->actions.size()) {
        switch(role) {
            case Qt::DisplayRole: return d->actions[row]->text();
            case Qt::DecorationRole: return d->actions[row]->icon();
            case Qt::ToolTip: return d->actions[row]->toolTip();
        }
    }
    return QVariant();
}

int PersonActions::rowCount(const QModelIndex& parent) const
{
    Q_D(const PersonActions);
    return parent.isValid() ? 0 : d->actions.size();
}

void PersonActions::trigger(int actionsRow)
{
    Q_D(PersonActions);
    d->actions[actionsRow]->trigger();
}

void PersonActions::setPerson(PersonData* data)
{
    Q_D(PersonActions);
    if(d->person == data)
        return;
    initialize(data->personIndex());
}

PersonData* PersonActions::person() const
{
    Q_D(const PersonActions);
    return d->person;
}
