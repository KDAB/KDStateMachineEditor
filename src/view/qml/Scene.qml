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
import QtQuick.Layouts 1.0

import com.kdab.kdsme 1.0

import "uml/"

StateMachineScene {
    id: root

    /// Debug: Draw filled semi-opaque rectangles around regions
    property bool debug: false

    width: rootState ? rootState.width : 0
    height: rootState ? rootState.height : 0

    transformOrigin: Item.TopLeft

    instantiator: RecursiveInstantiator {
        id: instantiator

        anchors.fill: parent

        model: root.model
        delegate: SceneItemFactory {
            scene: root
        }
    }
}
