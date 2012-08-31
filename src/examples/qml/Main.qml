import QtQuick 1.1
import org.kde.people 0.1
import org.kde.plasma.components 0.1
import org.kde.plasma.core 0.1 as Core
import org.kde.plasma.extras 0.1
import org.kde.qtextracomponents 0.1

Rectangle {
    width: 100
    height: 100
    color: "red"
    
    Core.SortFilterModel {
        id: filteredPeople
        sourceModel: PersonsModel { id: people }
        filterRegExp: searchField.text
    }
    
    TextField {
        id: searchField
        anchors {
            left: parent.left
            right: view.right
        }
    }
    GridView {
        id: view
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
                        height: view.cellHeight
                        width: view.cellWidth-5
                        Image {
                            id: avatar
                            source: photo[0]
                            anchors.fill: parent
                        }
                        Label {
                            width: parent.width
                            height: parent.height
                            clip: true
                            text: display
                            wrapMode: Text.WrapAnywhere
                            visible: avatar.status!=Image.Ready
                        }
                        enabled: true
                        onClicked: contactItem.contactData = model
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
                            id: actionsModel
                            row: contactItem.contactData!=null ? contactItem.contactData.index : -1
                            peopleModel: filteredPeople
                        }
                        delegate: Button {
                            text: model.display
                            iconSource: model.decoration
                            onClicked: actionsModel.trigger(model.index)
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
            property int index: contactItem.contactData.index
            
            buttonTexts: ["Unmerge", "Cancel"]
            titleText: i18n("Unmerging %1", unmergeDialog.name)
            content: Column {
                width: 500
                height: 200
                Repeater {
                    id: unmergesView
                    model: FullModelAccess {
                        rootIndex: indexFromModel(filteredPeople, unmergeDialog.index)
                        onRootIndexChanged: console.log("fuuuuuu", unmergesView.count)
                    }
                    delegate: ListItem {
                        property alias checked: willUnmerge.checked
                        property string contactUri: uri
                        enabled: true
                        onClicked: willUnmerge.checked=!willUnmerge.checked
                        Row {
                            spacing: 5
                            CheckBox { id: willUnmerge }
                            Label { text: display+" - "+contactId }
                        }
                    }
                }
            }
            onButtonClicked: if(index==0) {
                for(var i=0; i<unmergesView.count; ++i) {
                    var item = unmergesView.itemAt(i)
                    if(item.checked)
                        people.unmerge(item.contactUri)
                }
            }
        }
    }
}
