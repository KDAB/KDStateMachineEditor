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

pragma Singleton

import QtQuick 2.0

import com.kdab.kdsme 1.0

Item {
    property alias currentTheme: themeLoader.item

    readonly property string __currentThemeName: _quickView ? _quickView.themeName : "DarkTheme"

    Loader {
        id: themeLoader

        source: "qrc:///kdsme/qml/themes/" + __currentThemeName + ".qml"

        onStatusChanged: {
            if (status == Loader.Error)
                console.log("Failed to load theme: " + __currentThemeName)
        }
    }

    /**
     * Change alpha channel of color @p color by alpha value @p alpha
     */
    function alphaTint(color, alpha) {
        return Qt.rgba(color.r, color.g, color.b, alpha);
    }
}
