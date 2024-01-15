/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

import QtQuick 2.0

Item {
    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    readonly property color highlightBackgroundColor: myPalette.highlight;
    readonly property color highlightFontColor: myPalette.highlightedText;

    readonly property color compositeStateLabelBackgroundColor: myPalette.window;
    readonly property color compositeStateLabelFontColor: myPalette.text;
    readonly property color compositeStateBackgroundColor_Lightest: myPalette.midlight;
    readonly property color compositeStateBackgroundColor_Darkest: myPalette.dark;
    readonly property color stateBackgroundColor: myPalette.base;
    readonly property color stateBackgroundColor_Active: myPalette.highlight;
    readonly property color stateBorderColor: myPalette.text;
    readonly property color stateBorderColor_Active: myPalette.text;
    readonly property color stateBorderColor_Glow: Qt.lighter(myPalette.highlight, 0.4);
    readonly property bool  stateBorderColor_GlowEnabled: false;
    readonly property color stateLabelFontColor: myPalette.text;
    readonly property color transitionEdgeColor: myPalette.text;
    readonly property color transitionEdgeColor_Active: myPalette.highlight;
    readonly property color transitionLabelFontColor: myPalette.text;

    readonly property color viewBackgroundColor: myPalette.base;
    readonly property color viewAlternateBackgroundColor: myPalette.alternateBase;
}
