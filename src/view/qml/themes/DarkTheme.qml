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

QtObject {
    readonly property color highlightBackgroundColor: "#CACACA";
    readonly property color highlightFontColor: "white";

    readonly property color compositeStateLabelBackgroundColor: "#363636";
    readonly property color compositeStateLabelFontColor: "white";
    readonly property color compositeStateBackgroundColor_Lightest: "#666666";
    readonly property color compositeStateBackgroundColor_Darkest: "#202020";
    readonly property color stateBackgroundColor: "#262626";
    readonly property color stateBackgroundColor_Active: stateBackgroundColor;
    readonly property color stateBorderColor: "#BABABA";
    readonly property color stateBorderColor_Active: "#DADADA";
    readonly property color stateBorderColor_Glow: "#4589AA";
    readonly property bool  stateBorderColor_GlowEnabled: true;
    readonly property color stateLabelFontColor: "white";
    readonly property color transitionEdgeColor: "#BABABA";
    readonly property color transitionEdgeColor_Active: "#DADADA";
    readonly property color transitionLabelFontColor: "#BABABA";

    readonly property color viewBackgroundColor: "#262626";
    readonly property color viewAlternateBackgroundColor: "#545454";
}
