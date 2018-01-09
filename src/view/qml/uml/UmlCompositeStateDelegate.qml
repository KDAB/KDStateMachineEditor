/*
  UmlCompositeStateDelegate.qml

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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
import QtGraphicalEffects 1.0

import com.kdab.kdsme 1.0 as KDSME

import "qrc:///kdsme/qml/util/"

/**
 * Composite State representation
 */
Item {
    id: root

    property var control

    // Appearance
    readonly property real regionLabelMargins: scene.layoutProperties.regionLabelMargins
    readonly property size regionLabelButtonBoxSize: scene.layoutProperties.regionLabelButtonBoxSize

    readonly property bool active: control.activeness === 1.0
    readonly property bool expanded: control.element.expanded

    Behavior on width {
        SmoothedAnimation { duration: 200 }
    }
    Behavior on height {
        SmoothedAnimation { duration: 200 }
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

    UmlCompositeStateContent {
        id: rect

        anchors.fill: parent
    }

    Item {
        id: header

        clip: true

        width: parent.width
        height: text.height + 2*root.regionLabelMargins

        Rectangle {
            id: headerRect

            width: parent.width
            height: rect.height

            radius: rect.radius

            color: Theme.currentTheme.compositeStateLabelBackgroundColor
            border.color: rect.border.color
            border.width: rect.border.width
        }

        Rectangle {
            id: headerRectBottomBorder

            anchors.bottom: parent.bottom

            width: parent.width
            height: rect.border.width

            color: rect.border.color
        }

        Item {
            anchors {
                fill: parent
                margins: 2
                leftMargin: 5
            }

            Text {
                id: text

                anchors.centerIn: parent
                width: parent.width

                color: Theme.currentTheme.compositeStateLabelFontColor
                text: (control.element.label != "" ? control.element.label : qsTr("<Unnamed State>")) +
                    (control.element.childMode === KDSME.State.ParallelStates ? " " + qsTr("[parallel]") : "")
                renderType: Text.NativeRendering
                font.bold: true
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }

    RectangularSelectionHandler {
        anchors.fill: parent

        control: parent.control

        onDoubleClicked: {
            scene.rootState = control.element
            scene.layout()
        }
    }

    RowLayout {
        id: buttonBox

        visible: header.width > 1.1 * width

        width: root.regionLabelButtonBoxSize.width
        height: root.regionLabelButtonBoxSize.height

        anchors {
            top: header.top
            right: header.right
            margins: (header.height - height)/2
        }

        SquareButton {
            width: parent.height
            height: parent.width

            text: (element.expanded ? "-" : "+")
            onClicked: {
                scene.setItemExpanded(control.element, !element.expanded);
                scene.layout();
            }
        }
    }

    ChannelizedDropArea {
        id: dropArea
        anchors.fill: parent
        //keys: ["StateType", "TransitionType", "external"]
        element: control.element
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
