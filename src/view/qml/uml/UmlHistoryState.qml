/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sebastian Sauer <sebastian.sauer@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

import QtQuick 2.0
import QtQuick.Controls 2.12

import com.kdab.kdsme 1.0 as KDSME

import "../util/"

KDSME.StateItem {
    id: root

    Rectangle {
        id: rect

        anchors.fill: parent

        color: Theme.currentTheme.stateBackgroundColor
        border.color: Theme.currentTheme.stateBorderColor
        radius: width

        Text {
            anchors.centerIn: parent

            text: root.element.historyType === KDSME.HistoryState.ShallowHistory ? "H" : "H*"
            renderType: Text.NativeRendering
            color: Theme.currentTheme.stateLabelFontColor
            elide: Text.ElideRight
        }
    }

    RectangularSelectionHandler {
        anchors.fill: parent

        control: parent
    }


}
