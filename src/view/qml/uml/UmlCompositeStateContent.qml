/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
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

import com.kdab.kdsme 1.0 as KDSME

import "../util/functions.js" as Functions
import "../util/"

Rectangle {
    id: root

    property bool roundedCorners: true

    KDSME.DepthChecker {
        id: depthChecker

        target: element.parent
    }

    // Tint background color based on level in hierarchy, currently limited to 5
    color: Qt.tint(
        Qt.tint(Theme.currentTheme.compositeStateBackgroundColor_Lightest,
                Theme.alphaTint(Theme.currentTheme.compositeStateBackgroundColor_Darkest, depthChecker.depth.clamp(0, 5)/5.0)),
                Theme.alphaTint(Theme.currentTheme.highlightBackgroundColor, activeness.clamp(0, 3)/3.0)
        )
    border.color: Qt.tint(Theme.currentTheme.stateBorderColor, Theme.alphaTint(Theme.currentTheme.stateBorderColor_Active, activeness))
    border.width: (activeness > 0 ? 2 : 1)
    radius: roundedCorners ? 5 : 0

    Text {
        visible: !element.expanded

        anchors.centerIn: parent
        width: parent.width

        color: Theme.currentTheme.compositeStateLabelFontColor
        text: "..."
        renderType: Text.NativeRendering
        horizontalAlignment: Text.AlignHCenter
    }
}
