import QtQuick 2.0
import QtGraphicalEffects 1.0

RectangularGlow {
    id: effect

    visible: Theme.currentTheme.stateBorderColor_GlowEnabled && activeness > 0.0

    glowRadius: 10 * activeness
    spread: 0.1
    color: Theme.currentTheme.stateBorderColor_Glow
    cornerRadius: target.radius
}
