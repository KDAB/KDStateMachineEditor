/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

QtObject {
    readonly property color highlightForegroundColor: "#CACACA";
    readonly property color highlightBackgroundColor: "#545454";
    readonly property color activeHighlightColor: "gray";

    readonly property color compositeStateLabelBackgroundColor: "white";
    readonly property color compositeStateLabelFontColor: "black";
    readonly property color compositeStateBackgroundColor_Lightest: "#F2F2F2";
    readonly property color compositeStateBackgroundColor_Darkest: "#9B9DA3";
    readonly property color stateBackgroundColor: "white";
    readonly property color stateBackgroundColor_Active: "white";
    readonly property color stateBorderColor: "black";
    readonly property color stateBorderColor_Active: "#FF0000";
    readonly property color stateBorderColor_Glow: "#C20003";
    readonly property bool  stateBorderColor_GlowEnabled: false;
    readonly property color stateLabelFontColor: "black";
    readonly property color transitionEdgeColor: "black";
    readonly property color transitionEdgeColor_Active: "#FF0000";
    readonly property color transitionLabelFontColor: "black";
    readonly property color viewBackgroundColor: "white";
}

