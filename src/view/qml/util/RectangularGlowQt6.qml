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
