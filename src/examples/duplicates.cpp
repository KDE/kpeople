/*
    Duplicates Example
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
#include <QTreeView>
#include <QDebug>
#include <QTextStream>
#include <persons-model.h>
#include <duplicatesfinder.h>
#include <qtest_kde.h>

PersonsModel model;

class ResultPrinter : public QObject
{
    Q_OBJECT
    public slots:
        void print(KJob* j) {
            QList<Match> res = ((DuplicatesFinder* ) j)->results();
            qDebug() << "Results:";
            foreach(const Match& c, res) {
                QStringList roles;
                foreach(int i, c.role)
                    roles += model.roleNames()[i];
                QModelIndex idxA = model.index(c.rowA, 0), idxB = model.index(c.rowB, 0);
                qDebug() << "\t-" << roles.join(", ") << ":" << c.rowA << c.rowB << "because: " << idxA.data(c.role.first()) << idxB.data(c.role.first());
            }
            
            QCoreApplication::instance()->quit();
        }
};

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    
    ResultPrinter r;
    DuplicatesFinder* f = new DuplicatesFinder(&model);
    QObject::connect(f, SIGNAL(finished(KJob*)), &r, SLOT(print(KJob*)));
    f->start();
    
    app.exec();
}

#include "duplicates.moc"
