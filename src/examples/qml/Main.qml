import QtQuick 1.1
import org.kde.people 0.1
import org.kde.plasma.components 0.1
import org.kde.plasma.extras 0.1

Rectangle {
    width: 100
    height: 100
    color: "red"
    
    PersonsModel { id: people }
    
    ListView {
        id: view
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        width: parent.width/2
        
        model: people
        delegate:   ListItem {
                        height: lab.font.pixelSize*1.3
                        Label {
                            id: lab
                            anchors.centerIn: parent
                            width: parent.width
                            elide: Text.ElideRight
                            text: display
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
                    if(data==null)
                        return "";
                    
                    var text = ""
                    if(data.empty)
                        text="''"
                    for(var a in data) {
                        text += a + ": ";
                        var curr = data[a]
                        if(curr==null)
                            text += "null"
                        else if(!curr.isObject)
                            text += curr
                        else
                            text += dataToString[curr]
                        text += '\n'
                    }
                    return text
                }
            }
            Rectangle { color: "blue"; width: parent.width; height: 5}
            Flow {
                width: parent.width
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
            Rectangle { color: "green"; width: parent.width; height: 5 }
        }
    }
}
