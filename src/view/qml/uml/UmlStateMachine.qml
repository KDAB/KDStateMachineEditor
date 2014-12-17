/*
  UmlStateMachine.qml

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

    Rectangle {
        id: regionRect
        anchors.fill: parent

        color: Theme.compositeStateBackgroundColor_Lightest
        border.color: Theme.stateBorderColor
        border.width: 2

        Rectangle {
            id: textBox
            readonly property int margins: 8

            anchors {
                top: parent.top
                left: parent.left
            }
            width: text.width + 2*margins
            height: text.height + 2*margins

            color: Theme.compositeStateLabelBackgroundColor
            border.color: Theme.stateBorderColor
            border.width: 2

            Text {
                id: text
                anchors.centerIn: parent

                text: qsTr("State Machine: ") + (root.name != "" ? root.name : qsTr("<Unnamed>"))
                color: Theme.compositeStateLabelFontColor
                font.bold: true
                elide: Text.ElideRight
            }

        }
    }

    RectangularSelectionHandler {
        anchors.fill: parent

        control: parent
    }

    ChannelizedDropArea {
        id: dropArea
        anchors.fill: parent

        //keys: ["StateType", "HistoryStateType", "FinalStateType", "PseudoStateType"]
        keys: ["StateType"]

        element: root.element
    }

    states: [
        State {
            when: dropArea.containsDrag
            PropertyChanges {
                target: regionRect
                color: "#DDDDDD"
            }
        }
    ]

}
