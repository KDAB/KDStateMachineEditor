/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

