/*
  ChannelizedDropArea.qml

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

import com.kdab.kdsme 1.0

DropArea {
    id: root

    // A list of QMimeData.data's the DropArea accepts.
    keys: ["external"]

    // The UmlElement.item this DropArea is attached to.
    property variant item: null

    onEntered: {
        var sender = drag.source.dragData != undefined ? drag.source.dragData.item : null;
        var target = item
        if (!_quickView.editController.sendDragEnterEvent(sender, item, Qt.point(drag.x, drag.y), drag.urls)) {
            drag.accepted = false
        }
    }

    onDropped: {
        var sender = drop.source.dragData != undefined ? drop.source.dragData.item : null;
        if (!_quickView.editController.sendDropEvent(sender, item, Qt.point(drop.x, drop.y), drop.urls)) {
            drop.accepted = false
        }
    }
}
