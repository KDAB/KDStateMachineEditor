import QtQuick 2.0

Loader {
    id: root

    property Item target
    property real activeness

    readonly property bool isQt6: {
        try {
            return Qt.createQmlObject("import Qt5Compat.GraphicalEffects; RectangularGlow {}", root, "dynamicItem") !== null;
        } catch (error) {
            return false;
        }
    }

    anchors.fill: target

    source: isQt6 ? "qrc:/kdsme/qml/util/RectangularGlowQt6.qml" : "qrc:/kdsme/qml/util/RectangularGlowQt5.qml"
    onStatusChanged: console.log("Loader said: " + root.status + " " + source)

    Component.onCompleted: console.log("Loaded: " + source)
}
