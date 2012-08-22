import QtQuick 1.1
import org.kde.people 0.1

ListView {
    model: PersonsModel {}
    delegate: Text { text: display }
}