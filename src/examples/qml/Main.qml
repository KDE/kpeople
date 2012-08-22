import QtQuick 1.1
import org.kde.people 0.1

ListView {
    width: 100
    height: 100
    
    model: PersonsModel {}
    delegate: Text { text: display }
}