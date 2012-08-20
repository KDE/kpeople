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
#include <QTreeView>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <qpainter.h>
#include <../persons-model.h>

class ContactDelegate : public QStyledItemDelegate
{
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
        QStyledItemDelegate::paint(painter, option, index);
        if(index.parent().isValid())
            painter->drawText(option.rect.center(), index.data(PersonsModel::IMRole).toString());
    }
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    
    QTreeView view;
    view.setItemDelegate(new ContactDelegate);
    view.setModel(new PersonsModel(&view));
    view.setSortingEnabled(true);
    view.show();
    
    app.exec();
}