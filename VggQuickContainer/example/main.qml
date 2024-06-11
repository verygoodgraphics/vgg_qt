import QtQuick 2.0
import QtQuick.Window 2.0
import QVggQuickItem 1.0

Window {
    width: 1920
    height: 1080
    visible: true
    title: qsTr("VGG Qt Quick Demo")
    color: 'lightblue'

    QVggQuickItem {
        id: vgg
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 20
        width: 800
        height: 800

        fileSource: "/path/to/vgg/file"
    }
}