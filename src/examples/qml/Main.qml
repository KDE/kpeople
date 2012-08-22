import QtQuick 1.1
import org.kde.people 0.1
import org.kde.plasma.components 0.1
import org.kde.plasma.extras 0.1

Rectangle {
    width: 100
    height: 100
    color: "red"
    
    ListView {
        id: view
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        width: parent.width/2
        
        model: PersonsModel {}
        delegate:   ListItem {
                        Label {
                            anchors.fill: parent
                            elide: Text.ElideRight
                            text: display
                        }
                        enabled: true
                        onClicked: contactItem.contactData = model
                    }
    }
    
    Item {
        id: contactItem
        anchors {
            top: parent.top
            right: parent.right
            bottom: parent.bottom
        }
        width: parent.width/2
        property variant contactData
        function dataToString(data) {
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
        onContactDataChanged: {
            contactText.text = dataToString(contactData)
            console.log("contact selected:", contactText.text)
        }
        
        Label {
            id: contactText
            anchors.fill: parent
            text: "select a contact"
        }
    }
}
