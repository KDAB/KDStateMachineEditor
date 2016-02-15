/*
  UmlStateMachine.qml

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  This file may be distributed and/or modified under the terms of the
  GNU Lesser General Public License version 2.1 as published by the
  Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.

  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

  Contact info@kdab.com if any conditions of this licensing are not
  clear to you.
*/

import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

import com.kdab.kdsme 1.0 as KDSME

import "qrc:///kdsme/qml/util/"

/**
 * State Machine representation
 */
KDSME.StateItem {
    id: root

    width: Math.max(element.width, textBox.width)

    UmlCompositeStateContent {
        id: rect

        anchors.fill: parent

        roundedCorners: false

        Rectangle {
            id: textBox
            readonly property int margins: 8

            anchors {
                top: parent.top
                left: parent.left
            }
            width: Math.min(dummyLabel.width + 2*margins, element.width)
            height: dummyLabel.height + 2*margins

            color: Theme.currentTheme.compositeStateLabelBackgroundColor
            border.color: Theme.currentTheme.stateBorderColor
            border.width: 1

            Text {
                id: label

                anchors {
                    margins: 2
                    fill: parent
                }

                text: element.label !== "" ? element.label : qsTr("<Unnamed>")
                renderType: Text.NativeRendering
                color: Theme.currentTheme.compositeStateLabelFontColor
                font.bold: true
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                id: dummyLabel
                text: label.text
                renderType: Text.NativeRendering
                font.bold: true
                visible: false
            }
        }
    }

    RectangularSelectionHandler {
        anchors.fill: parent

        control: parent

        onDoubleClicked: {
            scene.rootState = control.element
            scene.layout()
        }
    }

    ChannelizedDropArea {
        id: dropArea
        anchors.fill: parent

        keys: ["StateType", "StateMachineType", "TransitionType"]

        element: root.element
    }

    states: [
        State {
            when: dropArea.containsDrag
            PropertyChanges {
                target: rect
                color: "#DDDDDD"
            }
        }
    ]

}
