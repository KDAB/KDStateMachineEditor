/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

import QtQuick 2.0
import Qt5Compat.GraphicalEffects

RectangularGlow {
    id: effect

    visible: Theme.currentTheme.stateBorderColor_GlowEnabled && activeness > 0.0

    glowRadius: 10 * activeness
    spread: 0.1
    color: Theme.currentTheme.stateBorderColor_Glow
    cornerRadius: target.radius
}
