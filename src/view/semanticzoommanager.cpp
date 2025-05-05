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

#include "semanticzoommanager.h"

#include "runtimecontroller.h"
#include "state.h"
#include "transition.h"
#include "elementwalker.h"
#include "statemachinescene.h"

using namespace KDSME;

SemanticZoomManager::SemanticZoomManager(QObject *parent)
    : QObject(parent)
    , m_scene(nullptr)
    , m_runtimeController(nullptr)
    , m_enabled(false)
{
}

StateMachineScene *SemanticZoomManager::scene() const
{
    return m_scene;
}

void SemanticZoomManager::setScene(StateMachineScene *scene)
{
    if (m_scene == scene)
        return;

    if (m_scene) {
        disconnect(m_scene, &StateMachineScene::rootStateChanged,
                   this, &SemanticZoomManager::updateRuntimeController);
    }
    m_scene = scene;
    if (m_scene) {
        connect(m_scene, &StateMachineScene::rootStateChanged,
                this, &SemanticZoomManager::updateRuntimeController);
    }
    updateRuntimeController();
    Q_EMIT sceneChanged(m_scene);
}

RuntimeController *SemanticZoomManager::runtimeController() const
{
    return m_runtimeController;
}

void SemanticZoomManager::setRuntimeController(RuntimeController *controller)
{
    if (m_runtimeController == controller)
        return;

    if (m_runtimeController) {
        disconnect(m_runtimeController, &RuntimeController::activeConfigurationChanged,
                   this, &SemanticZoomManager::handleActiveConfigurationChanged);
    }
    m_runtimeController = controller;
    if (m_runtimeController) {
        connect(m_runtimeController, &RuntimeController::activeConfigurationChanged,
                this, &SemanticZoomManager::handleActiveConfigurationChanged);
    }
}

bool SemanticZoomManager::isEnabled() const
{
    return m_enabled;
}

void SemanticZoomManager::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;

    m_enabled = enabled;
    Q_EMIT enabledChanged(m_enabled);
}

void SemanticZoomManager::handleActiveConfigurationChanged(const QSet<State *> &configuration)
{
    if (!m_enabled)
        return;

    auto root = m_scene->rootState();
    ElementWalker walker(ElementWalker::PreOrderTraversal);
    walker.walkChildren(root, [&](Element *i) -> ElementWalker::VisitResult {
        auto state = qobject_cast<State *>(i);
        if (!state) {
            return ElementWalker::RecursiveWalk;
        }

        const bool expand = configuration.contains(state);
        state->setExpanded(expand);
        return ElementWalker::RecursiveWalk;
    });

    m_scene->layout();
}

void SemanticZoomManager::updateRuntimeController()
{
    if (m_scene) {
        const auto machine = m_scene->rootState() ? m_scene->rootState()->machine() : nullptr;
        setRuntimeController(machine ? machine->runtimeController() : nullptr);
    }
}
