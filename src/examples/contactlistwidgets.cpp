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
#include <QSortFilterProxyModel>

#include <QVBoxLayout>
#include <QPushButton>

#include <QStyledItemDelegate>
#include <QPainter>

#include <personsmodel.h>

using namespace KPeople;

const int SPACING = 8;
const int PHOTO_SIZE = 32;

class PersonsDelegate : public QStyledItemDelegate
{
public:
    PersonsDelegate(QObject *parent = 0);
    ~PersonsDelegate();

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

PersonsDelegate::PersonsDelegate(QObject *parent)
:QStyledItemDelegate(parent)
{
}

PersonsDelegate::~PersonsDelegate()
{
}

void PersonsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 optV4 = option;
    initStyleOption(&optV4, index);

    painter->save();

    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
    painter->setClipRect(optV4.rect);

    QStyle *style = QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);

    QRect contactPhotoRect = optV4.rect;
    contactPhotoRect.adjust(SPACING, SPACING, SPACING, SPACING);
    contactPhotoRect.setWidth(PHOTO_SIZE);
    contactPhotoRect.setHeight(PHOTO_SIZE);

    QImage avatar = index.data(Qt::DecorationRole).value<QImage>();
    painter->drawImage(contactPhotoRect, avatar);

    painter->drawRect(contactPhotoRect);

    QRect nameRect = optV4.rect;
    nameRect.adjust(SPACING + PHOTO_SIZE + SPACING, SPACING, 0, 0);

    painter->drawText(nameRect, index.data(Qt::DisplayRole).toString());

    QRect idRect = optV4.rect;
    idRect.adjust(SPACING + PHOTO_SIZE + SPACING, SPACING + 15, 0, 0);
    painter->drawText(idRect, index.data(PersonsModel::PersonIdRole).toString());

    painter->restore();
}

QSize PersonsDelegate::sizeHint (const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(option)
    return QSize(128, 48);
}



class ContactListApp : public QWidget
{
    Q_OBJECT
public:
    ContactListApp();
private Q_SLOTS:
    void onMergeClicked();
    void onUnmergeClicked();
private:
    PersonsModel *m_model;
    QTreeView *m_view;
};

ContactListApp::ContactListApp()
{
    m_view = new QTreeView(this);
    m_model = new PersonsModel(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QSortFilterProxyModel *sortFilter = new QSortFilterProxyModel(m_view);
    sortFilter->setDynamicSortFilter(true);
    sortFilter->setSourceModel(m_model);
    sortFilter->sort(0);
    m_view->setRootIsDecorated(false);
    m_view->setModel(sortFilter);
    m_view->setItemDelegate(new PersonsDelegate(this));
    m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);

    layout->addWidget(m_view);
    QPushButton *mergeButton = new QPushButton("Merge", this);
    connect(mergeButton, SIGNAL(released()), SLOT(onMergeClicked()));
    layout->addWidget(mergeButton);

    QPushButton *unmergeButton = new QPushButton("Unmerge", this);
    connect(unmergeButton, SIGNAL(released()), SLOT(onUnmergeClicked()));
    layout->addWidget(unmergeButton);
}

void ContactListApp::onMergeClicked()
{
    QModelIndexList indexes = m_view->selectionModel()->selectedIndexes();
    QStringList ids;
    Q_FOREACH(const QModelIndex &index, indexes) {
        ids << index.data(PersonsModel::PersonIdRole).toString();
    }

    if (!ids.isEmpty()) {
        KPeople::mergeContacts(ids);
    }
}

void ContactListApp::onUnmergeClicked()
{
    QModelIndexList indexes = m_view->selectionModel()->selectedIndexes();
    if (indexes.size()) {
        QString id = indexes.first().data(PersonsModel::PersonIdRole).toString();
        KPeople::unmergeContact(id);
    }
}


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    ContactListApp widget;
    widget.show();
    app.exec();
}

#include "contactlistwidgets.moc"
