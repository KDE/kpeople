import QtQuick 2.1
import org.kde.people 1.0
import org.kde.plasma.components 2.0
import org.kde.plasma.core 2.0 as Core
import org.kde.plasma.extras 2.0
import org.kde.kquickcontrolsaddons 2.0

Rectangle {
    width: 300
    height: 300
    color: "red"

    Core.SortFilterModel {
        id: filteredPeople
        sortRole: "display"
        sortCaseSensitivity: Qt.CaseInsensitive
        sourceModel: PersonsModel {
            id: people
        }
        filterRegExp: searchField.text
    }

    CheckBox {
        id: areWeMerging
        anchors {
            top: parent.top
            left: parent.left
        }
        text: "Merging"
        onCheckedChanged: toMergeItems.clear()
    }

    TextField {
        id: searchField
        focus: true
        anchors {
            left: areWeMerging.right
            right: mergeLabel.left
        }
    }

    Label {
        id: mergeLabel
        anchors {
            top: parent.top
            right: view.right
        }
        ListModel {
            id: toMergeItems

            function uriIndex(uri) {
                var ret = -1
                for(var i=0; i<count && ret<0; ++i) {
                    if(get(i).uri==uri)
                        ret=i
                }
                return ret
            }

            function addUri(uri, name) {
                if(uriIndex(uri)<0)
                    toMergeItems.append({ "uri": uri, "name": name })
            }

            function indexes() {
                var ret = new Array
                for(var i=0; i<count; ++i) {
                    ret.push(get(i).uri)
                }
                return people.indexesForUris(ret)
            }

            function removeUri(uri) {
                remove(uriIndex(uri))
            }
        }
    }

    GridView {
        id: view
        clip: true
        anchors {
            top: searchField.bottom
            bottom: parent.bottom
            left: parent.left
            right: contactItem.left
        }

        cellWidth: 100
        cellHeight: 100
        model: filteredPeople
        delegate:   ListItem {
                        clip: true
                        height: view.cellHeight
                        width: view.cellWidth-5
                        Core.IconItem {
                            id: avatar
                            source: decoration
                            anchors.fill: parent
                        }
                        Label {
                            width: parent.width
                            height: parent.height
                            text: display
                            wrapMode: Text.WrapAnywhere
                            visible: avatar.status!=Image.Ready
                        }
                        enabled: true
                        onClicked: {
                            contactItem.contactData = model
                            personActions.personUri = model.personUri
                            if(areWeMerging.checked)
                                toMergeItems.addUri(model.personUri, model.display)
                        }
                    }
    }

    Flickable {
        id: contactItem
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
        width: parent.width/2
        property variant contactData

        Column {
            width: parent.width
            spacing: 5
            Column {
                visible: toMergeItems.count>0
                Label { text: "To Merge:" }
                Repeater {
                    model:toMergeItems
                    delegate: Label { text: name + " - " + uri }
                }
                Button {
                    text: "Merge!"
                    onClicked: {
                        people.createPersonFromIndexes(toMergeItems.indexes())
                        toMergeItems.clear()
                    }
                }
            }

            Label {
                id: contactText
                width: parent.width
                text: dataToString(contactItem.contactData)

                function dataToString(data) {
                    var text = ""
                    if(data==null)
                        return "<null>";
                    else for(var a in data) {
                        text += a + ": ";
                        var curr = data[a]
                        if(curr==null)
                            text += "null"
                        else
                            text += curr
                        text += '\n'
                    }
                    return text
                }
            }
            ToolBar {
                width: parent.width
                height: 30
                Flow {
                    anchors.fill: parent
                    Repeater {
                        model: PersonActions {
                            id: personActions
                        }
                        delegate: Button {
                            text: model.display
                            iconSource: model.decoration
                            onClicked: personActions.triggerAction(model.index)
                        }
                    }
                }
            }
            Rectangle { color: "green"; width: parent.width; height: 5 }
            Row {
                Repeater {
                    model: contactItem.contactData ? contactItem.contactData.photo : null
                    delegate: Image {
                        source: modelData
                    }
                }
            }
            Rectangle { color: "blue"; width: parent.width; height: 5}
            Button {
                text: "Unmerge"
                visible: contactItem.contactData!=null && contactItem.contactData.contactsCount>1
                onClicked: {
                    dialogLoader.sourceComponent = unmergeDialogComponent
                    dialogLoader.item.open()
                }
                Loader {
                    id: dialogLoader
                }
            }
        }
    }

    Component {
        id: unmergeDialogComponent
        CommonDialog {
            id: unmergeDialog
            property string name: contactItem.contactData.name
            property int index: filteredPeople.mapRowToSource(contactItem.contactData.index)
            property url uri: contactItem.contactData.uri

            buttonTexts: ["Unmerge", "Cancel"]
            titleText: i18n("Unmerging %1", unmergeDialog.name)
            content: Column {
                width: 500
                height: 200
                Repeater {
                    id: unmergesView
                    model: ColumnProxyModel {
                        rootIndex: indexFromModel(people, unmergeDialog.index)
                    }
                    delegate: ListItem {
                        property alias checked: willUnmerge.checked
                        property string contactUri: uri
                        enabled: true
                        onClicked: willUnmerge.checked=!willUnmerge.checked
                        Row {
                            spacing: 5
                            CheckBox { id: willUnmerge }
                            Label { text: display+" - "+contactUri }
                        }
                    }
                }
            }
            onButtonClicked: if(index==0) {
                for(var i=0; i<unmergesView.count; ++i) {
                    var item = unmergesView.itemAt(i)
                    if(item.checked)
                        people.unmerge(item.contactUri, uri)
                }
            }
        }
    }
}
