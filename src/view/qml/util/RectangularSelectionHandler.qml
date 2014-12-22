import QtQuick 2.0

import com.kdab.kdsme 1.0 as KDSME

import "qrc:///kdsme/qml/util/"

Item {
    id: root

    property var control

    signal clicked
    signal doubleClicked

    Rectangle {
        id: rect

        anchors.fill: parent

        border {
            color: Theme.highlightForegroundColor
            width: 2
        }
        color: "transparent"
        visible: control ?
            control.element.flags & KDSME.Element.ElementIsSelectable && control.element.selected :
            false
    }

    Item {
        id: invisibleDragTarget

        Drag.active: mouseArea.drag.active
        Drag.hotSpot.x: width/2
        Drag.hotSpot.y: height/2

        width: parent.width
        height: parent.height
    }

    MouseArea {
        id: mouseArea

        anchors.fill: invisibleDragTarget

        drag.target: invisibleDragTarget

        onClicked: {
            scene.currentItem = control.element;
            root.clicked();
        }
        onDoubleClicked: {
            root.doubleClicked();
        }

        function move(dx, dy) {
            var cmd = KDSME.CommandFactory.modifyLayoutItem(control.element);
            cmd.moveBy(dx, dy);
            commandController.push(cmd);
        }

        onXChanged: {
            move(x, 0);
        }
        onYChanged: {
            move(0, y);
        }
    }

}
