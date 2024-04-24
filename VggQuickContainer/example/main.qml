import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import VggQuickContainer

Window {
    width: 640
    height: 400
    visible: true
    title: qsTr("VGG Qt Quick Demo")

    VggQuickContainer {
        id: vgg
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 20
        width: 100
        height: 100
        fileSource: "/path/to/vgg/file"
    }
}
