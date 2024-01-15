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
import QtQuick.Layouts 1.0

/**
 * This is simply a horizontal or vertical line depending on @p layoutDirection
 * which can be used as separator element inside a Qt Quick Layout
 */
Item {
    id: root

    /**
     * How to layout this element
     *
     * E.g. in a RowLayout you may want to set this to Qt.Vertical in order to display a horizontal line
     *
     * Defaults to Qt.Horizontal
     */
    property int layoutDirection: Qt.Horizontal

    Layout.minimumHeight: 0
    Layout.minimumWidth: 0
    Layout.fillHeight: layoutDirection == Qt.Horizontal
    Layout.fillWidth: layoutDirection == Qt.Vertical

    Rectangle {
        id: style

        readonly property real extent: 0.6

        SystemPalette {
            id: myPalette
            colorGroup: SystemPalette.Disabled
        }

        anchors.centerIn: parent
        height: (parent.layoutDirection == Qt.Horizontal ? parent.height * extent : 1)
        width: (parent.layoutDirection == Qt.Vertical ? parent.width * extent : 1)

        color: myPalette.text
    }
}
