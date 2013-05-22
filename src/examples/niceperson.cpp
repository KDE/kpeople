/*
    Persons Model Example
    Copyright (C) 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

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
#include <QApplication>
#include <QWidget>
#include <QFormLayout>
#include <QLabel>
#include <QDebug>
#include <QToolButton>
#include <QMenu>
#include <persondata.h>
#include <personactions.h>

class PersonWidget : public QWidget
{
    Q_OBJECT
    public:
        explicit PersonWidget(const QString& contactId, QWidget* parent = 0, Qt::WindowFlags f = 0)
            : QWidget(parent)
            , m_person(contactId)
        {
            connect(&m_person, SIGNAL(dataInitialized()), SLOT(initGUI()));
        }

    private slots:
        void initGUI() {
            QFormLayout* l = new QFormLayout(this);
            l->addRow("contactId:", new QLabel(m_person.contactId()));
//             l->addRow("All Contacts:", new QLabel(m_person.bareContacts().join(", ")));
            l->addRow("nickname:", new QLabel(m_person.name()));
            l->addRow("status:", new QLabel(m_person.status()));

            QLabel* avatar = new QLabel;
            avatar->setPixmap(QPixmap(m_person.avatar().toLocalFile()));
            l->addRow("avatar:", avatar);

            PersonActions* actions = new PersonActions(&m_person);
//             actions->setPerson(); //TODO: figure out API to set a person
            QToolButton* b = new QToolButton;
            b->setText("Actions");
            QMenu* m = new QMenu(b);
            m->addActions(actions->actions());
            b->setPopupMode(QToolButton::MenuButtonPopup);
            b->setMenu(m);
            l->addRow("actions:", b);
        }

    private:
        PersonData m_person;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    if(app.argc()<2) {
        qWarning() << "Missing argument: \"" << qPrintable(app.arguments().first()) << " <contact id>\"";
        return 1;
    }

    PersonWidget w(app.arguments()[1]);

    w.show();
    app.exec();
}

#include "niceperson.moc"
