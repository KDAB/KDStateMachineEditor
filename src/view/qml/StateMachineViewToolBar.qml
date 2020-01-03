/*
  StateMachineViewToolBar.qml

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

import com.kdab.kdsme 1.0

import "qrc:///kdsme/qml/util"

import "util/constants.js" as Constants

ToolBar {
    id: root

    enabled: stateMachineScene.rootState

    /// Provide an instance of StateMachineView.qml here
    property var stateMachineView

    /// Internal
    readonly property var configurationController: (stateMachineView ? stateMachineView.configurationController : null)

    readonly property bool debuggingActive: configurationController ? configurationController.isRunning : false

    RowLayout {
        anchors.fill: parent

        ToolButton {
            action: Action {
                text: qsTr("Go Up")
                enabled: stateMachineScene.rootState && stateMachineScene.rootState.parentState()
                onTriggered: {
                    var parentState = stateMachineScene.rootState.parentState()
                    if (parentState) {
                        stateMachineScene.rootState = parentState
                        stateMachineScene.layout()
                    }
                }
            }
        }

        ToolButton {
            visible: root.debuggingActive
            action: Action {
                text: qsTr("Follow Active Region")
                checked: followActiveRegion
                checkable: true
                onToggled: {
                    stateMachineView.followActiveRegion = checked;
                    if (stateMachineView.followActiveRegion) {
                        stateMachineView.zoom = 1.0;
                        stateMachineView.centerOnActiveRegion();
                    }
                }
            }
        }
        /*
        ToolButton {
            visible: root.debuggingActive
            action: Action {
                text: qsTr("Enable Semantic Zoom")
                checked: stateMachineView.semanticZoom
                checkable: true
                onTriggered: {
                    stateMachineView.semanticZoom = !stateMachineView.semanticZoom
                }
            }
        }
        */
        ToolBarSeparator {}

        ToolButton {
            action: Action {
                text: qsTr("Fit in View")
                onTriggered: {
                    stateMachineView.fitInView();
                    stateMachineView.followActiveRegion = false;
                }
            }
        }
        ToolButton {
            action: Action {
                text: qsTr("Reset Zoom")
                onTriggered: {
                    stateMachineView.zoom = 1.0;
                    stateMachineView.followActiveRegion = checked;
                }
            }
        }

        Slider {
            id: zoomSlider

            Layout.maximumWidth: 100

            minimumValue: Constants.zoomMin
            maximumValue: Constants.zoomMax
            value: root.stateMachineView.zoom
            stepSize: Constants.zoomStepSize
            onValueChanged: root.stateMachineView.zoom = value
            Connections {
                target: root.stateMachineView
                onZoomChanged: zoomSlider.value = zoom
            }
        }
        Label {
            text: Math.round(stateMachineScene.zoom * 100) + " %"
        }

        ToolBarSeparator {}

        ToolButton {
            action: Action {
                text: qsTr("Show Transition Labels")
                checkable: true
                checked: stateMachineScene.layoutProperties.showTransitionLabels
                onTriggered: {
                    stateMachineScene.layoutProperties.showTransitionLabels = checked
                    stateMachineScene.layout()
                }
            }
        }

        Label {
            text: "Max. Depth:"
        }
        SpinBox {
            id: depthSlider

            Layout.maximumWidth: 40

            minimumValue: Constants.maximumDepthMin
            maximumValue: Constants.maximumDepthMax
            value: root.stateMachineView.maximumDepth
            stepSize: 1
            onValueChanged: {
                root.stateMachineView.maximumDepth = value;
            }
        }

        Item { Layout.fillWidth: true } // spacer
    }
}
