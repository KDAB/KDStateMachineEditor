/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.0

import com.kdab.kdsme 1.0

import "uml/"
import "util"

import "util/constants.js" as Constants

Rectangle {
    id: root
    objectName: "stateMachineView"

    property var rootState: stateMachineScene.rootState
    property var commandController: _quickView.commandController
    property var editController: _quickView.editController

    /// Whether we automatically move the content of the view-port in case the active region changes
    property bool followActiveRegion: false

    property alias maximumDepth: stateMachineScene.maximumDepth
    property alias zoom: stateMachineScene.zoom

    /// Whether we automatically collapse/expand states in case they're active or not
    property alias semanticZoom: semanticZoomManager.enabled

    enabled: editController.editModeEnabled || root.rootState
    focus: true

    Keys.onPressed: {
        if ((event.key === Qt.Key_Up) && (event.modifiers === Qt.NoModifier)) {
            stateMachineViewport.contentY = Math.max(0, stateMachineViewport.contentY - 10)
        } else if ((event.key === Qt.Key_Down) && (event.modifiers === Qt.NoModifier)) {
            var maximum = Math.max(0, stateMachineViewport.contentHeight - stateMachineViewport.height)
            stateMachineViewport.contentY = Math.min(maximum, stateMachineViewport.contentY + 10)
        } else if ((event.key === Qt.Key_Left) && (event.modifiers === Qt.NoModifier)) {
            stateMachineViewport.contentX = Math.max(0, stateMachineViewport.contentX - 10)
        } else if ((event.key === Qt.Key_Right) && (event.modifiers === Qt.NoModifier)) {
            var maximum = Math.max(0, stateMachineViewport.contentWidth - stateMachineViewport.width)
            stateMachineViewport.contentX = Math.min(maximum, stateMachineViewport.contentX + 10)
        } else if ((event.key === Qt.Key_Plus) && (event.modifiers & Qt.ControlModifier)) {
            setZoom(1.1, 0, 0)
        } else if ((event.key === Qt.Key_Minus) && (event.modifiers & Qt.ControlModifier)) {
            setZoom(0.9, 0, 0)
        } else if ((event.key === Qt.Key_Delete) && (event.modifiers === Qt.NoModifier)) {
            var cmd = CommandFactory.deleteElement(stateMachineScene, stateMachineScene.currentItem);
            commandController.push(cmd);
        }
    }

    color: Theme.currentTheme.viewBackgroundColor

    function setZoom(nominalFactor, centerX, centerY) {
        var scale = Math.min(Math.max(stateMachineScene.zoom * nominalFactor, Constants.zoomMin), Constants.zoomMax);
        var factor = scale / stateMachineScene.zoom;

        // calculate offset, to move the contents when zooming in or out to stay at the mouse position
        var pos = Qt.point(centerX, centerY);
        var newPos = Qt.point(pos.x * factor, pos.y * factor)
        var offsetX = newPos.x - pos.x;
        var offsetY = newPos.y - pos.y;

        stateMachineScene.zoom = scale;

        // if the horizontal scrollbar isn't visible (because displayed scene-width is smaller
        // then viewport) then not adjust contentX but keep it at zero. Means only start adjusting
        // the contentX once the zooming results in only parts being displayed.
        if (stateMachineViewport.visibleArea.widthRatio < 1) {
            stateMachineViewport.contentX = Math.max(0, stateMachineViewport.contentX + offsetX);
        } else {
            stateMachineViewport.contentX = 0
        }
        if (stateMachineViewport.visibleArea.heightRatio < 1) {
            stateMachineViewport.contentY = Math.max(0, stateMachineViewport.contentY + offsetY);
        } else {
            stateMachineViewport.contentY = 0
        }
    }

    function fitInView() {
        _quickView.fitInView()
    }

    function centerOnActiveRegion() {
        var activeRegion = configurationController.activeRegion;
        if (activeRegion.width === 0)
            return;

        var width = stateMachineViewport.width;
        var height = stateMachineViewport.height;
        var centerPos = Qt.point(width/2, height/2);
        stateMachineViewport.contentX = activeRegion.x - (centerPos.x - activeRegion.width/2);
        stateMachineViewport.contentY = activeRegion.y - (centerPos.y - activeRegion.height/2);
    }

    SemanticZoomManager {
        id: semanticZoomManager

        scene: stateMachineScene
    }

    Item {
        id: topDock

        width: parent.width
        height: childrenRect.height
        z: 1

        StateMachineViewToolBar {
            stateMachineView: root
        }
    }

    ScrollView {
        id: scrollView
        objectName: "scrollView"

        anchors {
            top: topDock.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        Flickable {
            id: stateMachineViewport
            objectName: "stateMachineViewport"

            anchors.fill: parent

            contentWidth: stateMachineScene.width * stateMachineScene.scale
            contentHeight: stateMachineScene.height * stateMachineScene.scale
            boundsBehavior: Flickable.StopAtBounds
            focus: true
            interactive: !editController.editModeEnabled

            onMovementStarted: {
                followActiveRegion = false;
            }

            Scene {
                id: stateMachineScene
                objectName: "stateMachineScene"

                Component.onCompleted: {
                    _quickView.scene = stateMachineScene
                }
            }

            Connections {
                target: (root.followActiveRegion ? root.configurationController : null)
                function onActiveRegionChanged() {
                    centerOnActiveRegion();
                }
            }

            Behavior on contentX {
                enabled: root.followActiveRegion
                SmoothedAnimation { duration: 200 }
            }
            Behavior on contentY {
                enabled: root.followActiveRegion
                SmoothedAnimation { duration: 200 }
            }

            // TODO: Add PinchArea?
        }
    }

    MouseArea {
        id: mouseArea

        readonly property alias view: stateMachineViewport
        readonly property alias scene: stateMachineScene

        anchors.fill: scrollView

        propagateComposedEvents: true
        hoverEnabled: true
        onWheel: {
            followActiveRegion = false;
            if (wheel.modifiers & Qt.ControlModifier) {
                var nominalFactor = (wheel.angleDelta.y > 0 ? 1.1 : 0.9);
                setZoom(nominalFactor, mouseX, mouseY);
            } else {
                wheel.accepted = false
            }
        }
        onPressed: {
            mouse.accepted = false
        }
    }

    BirdEyeView {
        id: birdEyeView

        width: 200
        height: 200

        opacity: canMove
        visible: opacity != 0.0
        flickable: stateMachineViewport
        scene: stateMachineScene

        anchors {
            bottom: parent.bottom
            right: parent.right
            margins: 20
        }

        Behavior on opacity {
            NumberAnimation { duration: 500 }
        }
    }

}
