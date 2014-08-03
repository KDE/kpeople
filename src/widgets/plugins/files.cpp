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

#include "files.h"

#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QStandardItem>
#include <KLocale>
#include <KPluginFactory>
#include <KPluginLoader>
#include <KLocalizedString>
#include <KABC/Addressee>
#include <LibKGAPI2/AuthJob>
#include <LibKGAPI2/Drive/File>
#include <LibKGAPI2/Drive/FileFetchJob>
#include <LibKGAPI2/Drive/FileSearchQuery>

using namespace KGAPI2;
using namespace KGAPI2::Drive;

Q_DECLARE_METATYPE(KGAPI2::Job *)

Files::Files(QObject *parent): AbstractFieldWidgetFactory(parent)
{

}


QWidget *Files::createDetailsWidget(const KABC::Addressee &person, const KABC::AddresseeList &contacts, QWidget *parent) const
{
    Q_UNUSED(contacts);
    QWidget *widget = new QWidget(parent);

    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    KConfig setting("kpeoplerc");
    KConfigGroup m_accountGroup = setting.group("GoogleAccount");
    const_cast<Files *>(this)->m_person = person;
    if (person.preferredEmail() == "") {
        layout->addWidget(new QLabel("No files"));
        widget->setLayout(layout);
        return widget;
    }

    if (m_accountGroup.exists() && m_accountGroup.hasKey("accountName") && m_accountGroup.hasKey("accessToken") && m_accountGroup.hasKey("refreshToken")) {

        KGAPI2::AccountPtr account(new KGAPI2::Account);
        account.data()->setAccountName(m_accountGroup.readEntry("accountName"));
        account.data()->setAccessToken(m_accountGroup.readEntry("accessToken"));
        account.data()->setRefreshToken(m_accountGroup.readEntry("refreshToken"));

        QStringList scopesList = m_accountGroup.readEntry("scopes").split(" ");
        QList<QUrl> scopes;
        foreach (QString s , scopesList) {
            scopes << QUrl(s);
        }
        account.data()->setScopes(scopes);
        const_cast<Files *>(this)->m_account = account;
        const_cast<Files *>(this)->getFiles();

    } else {
        QPushButton *auth = new QPushButton("Authenticate");
        layout->addWidget(auth);
        connect(auth, SIGNAL(clicked(bool)), this, SLOT(authenticate()));
    }

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(widget);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFixedHeight(widget->height());

    const_cast<Files *>(this)->m_ListView = new QListView();
    const_cast<Files *>(this)->m_model = new QStandardItemModel();

    m_ListView->setModel(m_model);
    m_ListView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_ListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_ListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(m_ListView);
    widget->setLayout(layout);
    return scrollArea;
}

void Files::authenticate()
{
    KGAPI2::AccountPtr account(new KGAPI2::Account);
    account->setScopes(QList<QUrl>() << QUrl("https://www.googleapis.com/auth/drive.readonly"));
    KGAPI2::AuthJob *authJob = new KGAPI2::AuthJob(
        account,
        QLatin1String("732262982909-n9aj72dft8vsa1kr85n93vmjehu2q0oq.apps.googleusercontent.com"),
        QLatin1String("bbwbrU4ym4UBF08brgSey-1c"));
    connect(authJob, SIGNAL(finished(KGAPI2::Job *)), this, SLOT(slotAuthJobFinished(KGAPI2::Job *)));
}

void Files::updateAccountToken(const AccountPtr &account, Job *restartJob)
{
    KGAPI2::AuthJob *authJob = new KGAPI2::AuthJob(
        account,
        QLatin1String("732262982909-n9aj72dft8vsa1kr85n93vmjehu2q0oq.apps.googleusercontent.com"),
        QLatin1String("bbwbrU4ym4UBF08brgSey-1c"));
    authJob->setProperty(JOB_PROPERTY, QVariant::fromValue(restartJob));
    connect(authJob, SIGNAL(finished(KGAPI2::Job *)),
            this, SLOT(slotAuthJobFinished(KGAPI2::Job *)));
}


void Files::slotAuthJobFinished(KGAPI2::Job *job)
{
    KGAPI2::AuthJob *authJob = qobject_cast<KGAPI2::AuthJob *>(job);
    Q_ASSERT(authJob);
    authJob->deleteLater();
    if (authJob->error() != KGAPI2::NoError) {
        qDebug() << "Error" + authJob->errorString();
        return;
    }
    m_account = authJob->account();

    KConfig setting("kpeoplerc");
    KConfigGroup m_accountGroup = setting.group("GoogleAccount");
    m_accountGroup.writeEntry("accountName", m_account.data()->accountName());
    m_accountGroup.writeEntry("accessToken", m_account.data()->accessToken());
    m_accountGroup.writeEntry("refreshToken", m_account.data()->refreshToken());
    QString scopes;
    foreach (QUrl url , m_account.data()->scopes()) {
        scopes += url.toString() + " ";
    }
    scopes = scopes.trimmed();
    m_accountGroup.writeEntry("scopes", scopes);
    m_accountGroup.sync();

    getFiles();
}

void Files::getFiles()
{
    FileSearchQuery query(FileSearchQuery::Or);
    query.addQuery(FileSearchQuery::Readers, FileSearchQuery::In, m_person.preferredEmail());
    query.addQuery(FileSearchQuery::Writers, FileSearchQuery::In, m_person.preferredEmail());

    KGAPI2::Drive::FileFetchJob *fetchJob = new KGAPI2::Drive::FileFetchJob(query, m_account);
    connect(fetchJob, SIGNAL(finished(KGAPI2::Job *)), this, SLOT(slotFileFetchJobFinished(KGAPI2::Job *)));
}


void Files::slotFileFetchJobFinished(KGAPI2::Job *job)
{


    KGAPI2::Drive::FileFetchJob *fetchJob = qobject_cast<KGAPI2::Drive::FileFetchJob *>(job);
    Q_ASSERT(fetchJob);
    fetchJob->deleteLater();

    if (fetchJob->error() == KGAPI2::Unauthorized) {
        qDebug() << "Error:u:" + fetchJob->errorString();
        updateAccountToken(m_account, job);
    }

    if (fetchJob->error() != KGAPI2::NoError) {
        qDebug() << "Error:d" + fetchJob->errorString();
        qDebug() << fetchJob->error();
        return ;
    }



    qDebug() << "Got Files List";
    /* Get all items the job has retrieved */
    const KGAPI2::ObjectsList objects = fetchJob->items();

    Q_FOREACH(const KGAPI2::ObjectPtr & object, objects) {
        const KGAPI2::Drive::FilePtr file = object.dynamicCast<KGAPI2::Drive::File>();
        /* Convert the contact to QListWidget item */
        qDebug() << file.data()->isFolder();
        qDebug() << file.data()->title();
        qDebug() << file.data()->embedLink(); //Read Only Link for files
        qDebug() << file.data()->alternateLink();
        qDebug() << file.data()->thumbnailLink();//Files only
//         qDebug() << file.data()->thumbnail();
        
        QStandardItem *item = new QStandardItem();
        item->setData(file.data()->title(), Qt::DisplayRole);
        m_model->appendRow(item);
    }

}


QString Files::label() const
{
    return i18n("Files");
}

int Files::sortWeight() const
{
    return 0;
}

#include "files.moc"

