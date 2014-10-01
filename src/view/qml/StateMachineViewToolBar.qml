/*
  StateMachineViewToolBar.qml

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

import com.kdab.kdsme 1.0

import "util/constants.js" as Constants

ToolBar {
    id: root

    /// Provide an instance of StateMachineView.qml here
    property var stateMachineView

    RowLayout {
        anchors.fill: parent

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

            minimumValue: Constants.minimumZoomLevel
            maximumValue: Constants.maximumZoomLevel
            value: 1
            tickmarksEnabled: true
            stepSize: Constants.zoomStepSize
            onValueChanged: {
                if (!root.stateMachineView)
                    return;

                root.stateMachineView.zoom = value;
            }

            Connections {
                target: root.stateMachineView
                onZoomChanged: zoomSlider.value = zoom
            }
        }
    }
}
