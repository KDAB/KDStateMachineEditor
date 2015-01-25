/*
  UmlPseudoState.qml

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

import com.kdab.kdsme 1.0

import "qrc:///kdsme/qml/util"

StateItem {
    id: root

    property int kind: element.kind

    readonly property real originalWidth: 30

    opacity: 2*width > originalWidth

    Behavior on opacity {
        NumberAnimation { duration: 100; easing.type: Easing.InOutQuad }
    }

    Loader {
        id: loader
        anchors.fill: parent

        sourceComponent: {
            if (kind == PseudoState.InitialState) {
                return initialState;
            } else {
                console.log("UmlPseudoState: Unknown kind: " + kind);
                return null;
            }
        }
    }

    Component {
        id: initialState

        Rectangle {
            anchors.fill: parent

            color: Theme.stateBackgroundColor
            border.color: Theme.stateBorderColor
            radius: width
        }

    }
}
