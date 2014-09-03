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

import "qrc:///kdsme/qml/util/"
import "qrc:///kdsme/qml/util/theme.js" as Theme

/**
 * Composite State representation
 */
Rectangle {
    id: root

    // Appearance
    readonly property real regionLabelMargins: view.layoutProperties.regionLabelMargins
    readonly property size regionLabelButtonBoxSize: view.layoutProperties.regionLabelButtonBoxSize

    readonly property bool expanded: control.item.expanded

    // TODO: Tint background color based on level in hierarchy
    color: Theme.compositeStateLabelBackgroundColor
    readonly property color activeBorderColor: Theme.compositeStateBorderColor_Active // only needed to force type conversion to a color object
    border.color: Qt.tint(Theme.compositeStateBorderColor, Qt.rgba(activeBorderColor.r, activeBorderColor.g, activeBorderColor.b, activeness))
    border.width: (activeness > 0 ? 2 : 1)
    radius: 5

    Behavior on width {
        SmoothedAnimation { duration: 200 }
    }
    Behavior on height {
        SmoothedAnimation { duration: 200 }
    }

    Item {
        id: header

        anchors {
            top: parent.top
        }

        width: parent.width
        height: text.height + 2*root.regionLabelMargins

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
                text: (control.name != "" ? control.name : qsTr("<Unnamed State>"))
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
                    text: (control.expanded ? "-" : "+")
                    onClicked: {
                        if (control.expanded) {
                            view.collapseItem(control.item)
                        } else {
                            view.expandItem(control.item)
                        }
                    }
                }
                */
            }
        }
    }

    Rectangle {
        id: contentsRect

        anchors {
            top: header.bottom
            bottom: parent.bottom
        }

        width: parent.width

        color: Theme.compositeStateBackgroundColor_Lightest
        border.color: root.border.color
        border.width: root.border.width

        clip: true
    }

    ChannelizedDropArea {
        id: dropArea
        anchors.fill: parent
        //keys: ["StateType", "TransitionType", "external"]
        item: control.item
    }

    states: [
        State {
            when: dropArea.containsDrag
            PropertyChanges {
                target: contentsRect
                color: "#DDDDDD"
            }
        }
    ]

}
