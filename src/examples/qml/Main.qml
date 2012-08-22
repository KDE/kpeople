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
        onContactDataChanged: {
            var text = ""
            for(var a in contactData)
                text += a + ": " + contactData[a]+'\n'
            contactText.text = text
            console.log("contact selected:", text)
        }
        
        Label {
            id: contactText
            anchors.fill: parent
            text: "select a contact"
        }
    }
}
