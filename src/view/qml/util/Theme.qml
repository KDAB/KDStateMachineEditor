/*
  Theme.qml

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

pragma Singleton

import QtQuick 2.0

import com.kdab.kdsme 1.0

Item {
    property alias currentTheme: themeLoader.item

    readonly property string __currentThemeName: _quickView.themeName

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
