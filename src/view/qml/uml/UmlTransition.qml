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

import com.kdab.kdsme 1.0

import "../util/"

TransitionItem {
    id: root

    property rect labelRect: element.labelBoundingRect

    width: container.width
    height: container.height

    shape: element.shape
    visible: !painterPathItem.isEmpty

    opacity: (element.sourceState ? element.sourceState.visible : 1) &&
        (element.targetState ? element.targetState.visible : 1)

    PainterPath {
        id: painterPathItem
        path: root.shape
    }

    MaskedMouseArea {
        id: mouseArea

        anchors.fill: parent

        mask: PainterPathMask {
            path: root.shape
        }
        tolerance: 5

        onClicked: {
            scene.currentItem = element
        }
    }

    Primitive {
        id: selectionComponent

        PainterPathStroker {
            id: outlinePathStroker

            width: 10
        }

        visible: element.selected

        geometry: PainterPathGeometry {
            path: outlinePathStroker.createStroke(root.shape)
        }

        color: Theme.alphaTint(Theme.currentTheme.transitionEdgeColor, 0.5)
    }

    Item {
        id: container

        width: childrenRect.width
        height: childrenRect.height

        Primitive {
            id: edge

            geometry: PainterPathGeometry {
                path: root.shape
                lineWidth: (activeness > 0 ? 2 : 1)
            }

            color: Qt.tint(Theme.currentTheme.transitionEdgeColor, Theme.alphaTint(Theme.currentTheme.transitionEdgeColor_Active, activeness))

            ArrowHead {
                function angle() {
                     // bind to changes to the start/end point
                    painterPathItem.startPoint
                    painterPathItem.endPoint
                    return -painterPathItem.angleAtPercent(1.0)
                }

                transformOrigin: Item.Right
                x: painterPathItem.endPoint.x-width
                y: painterPathItem.endPoint.y-height/2
                rotation: (!painterPathItem.isEmpty ? angle() : 0.)

                width: 6
                height: 6

                color: parent.color
            }
        }

        Text {
            x: labelRect.x
            y: labelRect.y
            width: labelRect.width
            height: labelRect.height
            color: Theme.currentTheme.transitionLabelFontColor
            font.italic: true
            visible: scene.layoutProperties.showTransitionLabels
                && element.sourceState.type != Element.PseudoStateType
                && labelRect.height > 0 && element.label != ""
                && scene.zoom >= 1.0

            text: element.label
            renderType: Text.NativeRendering
        }
    }

    DragPointGroup {
        id: handles

        visible: element.flags & Element.ElementIsDragEnabled && element.selected

        dragKeys: ["TransitionType"]
        dragData: root

        points: [
            DragPoint { x: painterPathItem.startPoint.x;    y: painterPathItem.startPoint.y },
            DragPoint { x: painterPathItem.endPoint.x;      y: painterPathItem.endPoint.y }
        ]

        onChanged: {
            var start = pointAt(0);
            var end = pointAt(1);
            var path = Global.createPath(start);
            path.lineTo(end);

            var cmd = CommandFactory.modifyTransition(root.element);
            cmd.setShape(path.path);
            commandController.push(cmd);
        }
        onDropped: {
            var transition = root.element;
            while (target && target.parent && (target.element == undefined || target.element == undefined)) { // find the state containing the drop area
                target = target.parent;
            }
            // If target == null => reparent to root state (the state machine object)
            var state = (target ? target.element : root.element.machine());
            if (index == 0) {
                if (transition.sourceState != state) {
                    var cmd = CommandFactory.reparentElement(view, transition);
                    cmd.setParentElement(state);
                    commandController.push(cmd)
                }
            } else if (transition.targetState != state) {
                var cmd = CommandFactory.modifyTransition(transition);
                cmd.setTargetState(state);
                commandController.push(cmd)
            }
        }
    }

}
