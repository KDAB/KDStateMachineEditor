/*
  BirdEyeView.qml

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

import "qrc:///kdsme/qml/util/functions.js" as Functions
import "qrc:///kdsme/qml/util/"

Item {
    id: root

    /// Flickable instance (viewport)
    property variant flickable: null
    /// Item which is shown in the Flickable instance
    property variant scene: null

    /// Whether we can move the scene rect inside the view rect
    readonly property bool canMove: (sceneRect.width > viewRect.width || sceneRect.height > viewRect.height)

    /// Scale factor for translation from flickable/scene coordinates to birdeye-view coordinates
    property real nominalScale: Math.min(width / (scene.width/scene.scale), height / (scene.height/scene.scale))

    /**
     * Move Flickable viewport to position defined by contentX and contentY
     *
     * @note Respects content size and does not move beyond bonds
     */
    function moveViewport(contentX, contentY) {
        root.flickable.contentX = contentX.clamp(0, Math.max(0, flickable.contentWidth - flickable.width))
        root.flickable.contentY = contentY.clamp(0, Math.max(0, flickable.contentHeight - flickable.height))
    }

    Rectangle {
        id: sceneRect

        anchors.centerIn: parent
        width: (scene.width/scene.scale) * root.nominalScale
        height: (scene.height/scene.scale) * root.nominalScale

        opacity: 0.8
        color: Theme.currentTheme.highlightBackgroundColor
        border.color: Theme.currentTheme.highlightForegroundColor
        border.width: 1

        MouseArea {
            anchors.fill: parent

            onClicked: {
                // center the view rect around the mouse click position
                var topLeftX = mouseX - viewRect.width/2
                var topLeftY = mouseY - viewRect.height/2
                var relX = topLeftX / sceneRect.width
                var relY = topLeftY / sceneRect.height
                var contentX = (relX) * root.flickable.contentWidth
                var contentY = (relY) * root.flickable.contentHeight
                moveViewport(contentX, contentY)
            }
        }

        ShaderEffectSource {
            id: source

            anchors.fill: parent

            opacity: 0.8
            sourceItem: root.scene
            mipmap: true
            recursive: false
        }

        Rectangle {
            id: viewRect

            property real currentX: 0
            property real currentY: 0

            x: (root.flickable.visibleArea.xPosition * sceneRect.width).clamp(0, sceneRect.width)
            y: (root.flickable.visibleArea.yPosition * sceneRect.height).clamp(0, sceneRect.height)
            width: (root.flickable.visibleArea.widthRatio * sceneRect.width).clamp(0, sceneRect.width - x)
            height: (root.flickable.visibleArea.heightRatio * sceneRect.height).clamp(0, sceneRect.height - y)

            color: "#10ffffff"
            border.color: Theme.currentTheme.highlightForegroundColor
            border.width: 2
            visible: flickable

            MouseArea {
                anchors.fill: parent

                property real offsetX: 0
                property real offsetY: 0

                propagateComposedEvents: true
                hoverEnabled: true
                cursorShape: (containsMouse ? Qt.SizeAllCursor : Qt.ArrowCursor)

                onPressed: {
                    offsetX = mouseX
                    offsetY = mouseY
                }
                onPositionChanged: {
                    if (pressed) {
                        var deltaX = mouseX - offsetX
                        var deltaY = mouseY - offsetY
                        var contentX = (root.flickable.contentX + (deltaX / root.scale) * root.scene.scale)
                        var contentY = (root.flickable.contentY + (deltaY / root.scale) * root.scene.scale)
                        moveViewport(contentX, contentY)
                    }
                    mouse.accepted = true

                }
                onClicked: {
                    mouse.accepted = true
                }
                onWheel: {
                    wheel.accepted = false;
                }
            }
        }
    }
}
