import QtQuick 1.1
import org.kde.people 0.1
import org.kde.plasma.components 0.1
import org.kde.plasma.extras 0.1

Rectangle {
    width: 100
    height: 100
    color: "red"
    
    PersonsModel { id: people }
    
    GridView {
        id: view
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: contactItem.left
        }
        
        cellWidth: 100
        cellHeight: 100
        model: people
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
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
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
                            id: actionsModel
                            row: contactItem.contactData!=null ? contactItem.contactData.index : -1
                            peopleModel: people
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
        }
    }
}
