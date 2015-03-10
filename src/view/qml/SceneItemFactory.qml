/*
  SceneItemFactory.qml

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
import QtQuick.Layouts 1.0

import com.kdab.kdsme 1.0

import "uml/"

Loader {
    id: root

    property var scene

    readonly property var runtimeController: scene.rootState ?
        scene.rootState.machine().runtimeController :
        null

    x: object.pos.x
    y: object.pos.y

    active: object
    asynchronous: true

    sourceComponent: {
        if (object.type === Element.StateMachineType) {
            return umlStateMachine;
        } else if (object.type === Element.FinalStateType) {
            return umlFinalState;
        } else if (object.type === Element.HistoryStateType) {
            return umlHistoryState;
        } else if (object.type === Element.PseudoStateType) {
            return umlPseudoState;
        } else if (object.type === Element.StateType) {
            return umlState;
        } else if (object.type === Element.TransitionType) {
            return umlTransition;
        } else if (object.type === Element.SignalTransitionType) {
            return umlTransition; // no special representation for now
        } else if (object.type === Element.TimeoutTransitionType) {
            return umlTransition; // no special representation for now
        } else {
            console.log("Unknown Element type: " + object.type);
            return null;
        }
    }

    function activenessForState(state) {
        if (!runtimeController)
            return 0;

        runtimeController.activeConfiguration; // bind to active configuration
        return runtimeController.activenessForState(state);
    }

    function activenessForTransition(transition) {
        if (!runtimeController)
            return 0;

        runtimeController.activeConfiguration; // bind to active configuration
        return runtimeController.activenessForTransition(transition)
    }

    Component {
        id: umlState
        UmlState {
            scene: root.scene
            element: object
            activeness: activenessForState(object)
        }
    }

    Component {
        id: umlStateMachine
        UmlStateMachine {
            scene: root.scene
            element: object
        }
    }

    Component {
        id: umlFinalState
        UmlFinalState {
            scene: root.scene
            element: object
            activeness: activenessForState(object)
        }
    }

    Component {
        id: umlHistoryState
        UmlHistoryState {
            scene: root.scene
            element: object
            activeness: activenessForState(object)
        }
    }

    Component {
        id: umlPseudoState
        UmlPseudoState {
            scene: root.scene
            element: object
            activeness: activenessForState(object)
        }
    }

    Component {
        id: umlTransition
        UmlTransition {
            scene: root.scene
            element: object
            activeness: activenessForTransition(object)
        }
    }
}
