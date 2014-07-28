/*
  StateMachineScene.qml

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

import "uml/"

Item {
    id: root

    property var currentView: null
    property var configurationController: null

    /// Debug: Draw filled semi-opaque rectangles around regions
    property bool debug: false

    width: ((currentView && currentView.rootLayoutItem) ? currentView.rootLayoutItem.width : 0)
    height: ((currentView && currentView.rootLayoutItem) ? currentView.rootLayoutItem.height : 0)

    transformOrigin: Item.TopLeft

    LayoutInformationModel {
        id: layoutInformationModel
        view: (root.currentView ? root.currentView : null)
    }

    Component {
        id: recursiveDelegate

        Repeater {
            model: VisualDataModel {
                id: visualDataModel

                model: layoutInformationModel
                rootIndex: (typeof(myRootIndex) === 'undefined' ? visualDataModel.rootIndex : myRootIndex)
                delegate: LayoutItemLoader {
                    id: delegate

                    view: root.currentView
                    configurationController: root.configurationController

                    Loader {
                        property variant myRootIndex: visualDataModel.modelIndex(index)
                        anchors {
                            left: parent.left
                            top: parent.top
                        }
                        asynchronous: true
                        sourceComponent: (hasModelChildren ? recursiveDelegate : undefined)
                        Component.onDestruction: {
                            // force unloading the source component
                            sourceComponent = undefined
                        }
                    }

                    Rectangle {
                        id: debugBoundingRect

                        anchors.fill: parent
                        visible: root.debug
                        color: (object.selected ? "green" : "red")
                        opacity: 0.5
                    }
                }
            }
        }
    }

    Loader {
        id: loader

        sourceComponent: recursiveDelegate
    }
}
