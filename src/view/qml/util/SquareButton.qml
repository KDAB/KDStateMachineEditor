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

MouseArea {
    id: button

    property alias text: textItem.text

    hoverEnabled: true

    Rectangle {
        id: style

        anchors.fill: parent

        border.color: Theme.currentTheme.stateBorderColor
        radius: 5
        color: !button.pressed ?
            Theme.currentTheme.compositeStateLabelBackgroundColor :
            Theme.currentTheme.compositeStateBackgroundColor_Darkest

        Text {
            id: textItem

            anchors.centerIn: parent

            color: Theme.currentTheme.compositeStateLabelFontColor
            renderType: Text.NativeRendering
        }
    }

}
