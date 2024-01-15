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

Item {
    id: root

    /// Whether this accepts mouse events
    property bool enabled: true

    /// The model of this group
    property list<DragPoint> points

    /// @see QtQuick.Drag.keys
    property var dragKeys: []

    /// Data that will be passe
    property var dragData

    /**
     * Returns the point at index @p index in the coordinate system
     * of the root QML view
     *
     * @return type:Qt.point
     */
    function pointAt(index) {
        var element = repeater.itemAt(index);
        var x = element.x+element.width/2;
        var y = element.y+element.height/2;
        return Qt.point(x, y);
    }

    /**
     * Emitted whenever one of the drag point delegates is moved
     *
     * Call pointAt(index) in order to find out the current position
     * of the changed drag point
     */
    signal changed(int index)

    /**
     * Emitted whenever one of the drag point delegates
     * is dropped onto a DropArea
     */
    signal dropped(int index, var target)

    Repeater {
        id: repeater

        model: points
        delegate: DragPointDelegate {
            enabled: root.enabled
            dragData: root.dragData
            dragIndex: index

            Drag.keys: root.dragKeys

            x: modelData.x-width/2
            y: modelData.y-height/2

            onXChanged: { if (Drag.active) root.changed(index) }
            onYChanged: { if (Drag.active) root.changed(index) }
            onDropped: { root.dropped(index, target) }
        }
    }

}
