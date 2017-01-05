/*
  UmlFinalState.qml

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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
import QtGraphicalEffects 1.0

import com.kdab.kdsme 1.0 as KDSME

import "qrc:///kdsme/qml/util/"

/**
 * Final State representation
 *
 * This is a node in the form of a double-circle
 */
KDSME.StateItem {
    id: root

    readonly property bool active: activeness === 1.0

    readonly property real originalWidth: 20

    opacity: 2*width > originalWidth

    Behavior on opacity {
        NumberAnimation { duration: 100; easing.type: Easing.InOutQuad }
    }

    RectangularGlow {
        id: effect

        anchors.fill: rect
        visible: Theme.currentTheme.stateBorderColor_GlowEnabled && active

        glowRadius: 10
        spread: 0.1
        color: Theme.currentTheme.stateBorderColor_Glow
        cornerRadius: rect.radius
    }

    Rectangle {
        id: rect

        anchors.fill: parent

        color: Theme.currentTheme.stateBackgroundColor
        border.color: Qt.tint(Theme.currentTheme.stateBorderColor, Theme.alphaTint(Theme.currentTheme.stateBorderColor_Active, activeness))
        border.width: (activeness > 0 ? 2 : 1)
        radius: width

        Rectangle {
            anchors {
                fill: parent
                margins: 5
            }

            color: Qt.darker(parent.border.color, 1.5)
            radius: parent.radius*0.5
        }
    }

    RectangularSelectionHandler {
        anchors.fill: parent

        control: parent
    }

    ChannelizedDropArea {
        id: dropArea
        anchors.fill: parent
        keys: ["TransitionType"]
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
