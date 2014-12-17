/*
  UmlCompositeStateDelegate.qml

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
    readonly property real regionLabelMargins: view.layoutProperties.regionLabelMargins
    readonly property size regionLabelButtonBoxSize: view.layoutProperties.regionLabelButtonBoxSize

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
        visible: active

        glowRadius: 10
        spread: 0.1
        color: Theme.stateBorderColor_Glow
        cornerRadius: rect.radius
    }

    Rectangle {
        id: rect

        anchors.fill: parent

        // TODO: Tint background color based on level in hierarchy
        color: Theme.compositeStateBackgroundColor_Lightest
        border.color: Qt.tint(Theme.stateBorderColor, Theme.alphaTint(Theme.stateBorderColor_Active, activeness))
        border.width: (activeness > 0 ? 2 : 1)
        radius: 5
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

            color: Theme.compositeStateLabelBackgroundColor
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

                property bool leftAlign: (paintedWidth + 10 > width - 2*buttonBox.width)

                anchors.centerIn: parent
                width: parent.width

                color: Theme.compositeStateLabelFontColor
                text: (control.element.label != "" ? control.element.label : qsTr("<Unnamed State>"))
                font.bold: true
                elide: Text.ElideRight
                horizontalAlignment: (leftAlign ? Text.AlignLeft : Text.AlignHCenter)
            }

            RowLayout {
                id: buttonBox

                width: root.regionLabelButtonBoxSize.width
                height: root.regionLabelButtonBoxSize.height

                anchors {
                    top: parent.top
                    right: parent.right
                }

                // TODO: Re-evaluate if we need this functionality
                /*
                SquareButton {
                    width: parent.height
                    height: parent.width
                    text: (element.expanded ? "-" : "+")
                    onClicked: {
                        if (element.expanded) {
                            view.collapseItem(element.element)
                        } else {
                            view.expandItem(element.element)
                        }
                    }
                }
                */
            }
        }
    }

    RectangularSelectionHandler {
        anchors.fill: parent

        control: parent.control
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
