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

#ifndef KDSME_SEMANTICZOOMMANAGER_H
#define KDSME_SEMANTICZOOMMANAGER_H

#include "kdsme_view_export.h"

#include <QObject>

namespace KDSME {

class RuntimeController;
class State;
class StateMachineScene;

class KDSME_VIEW_EXPORT SemanticZoomManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KDSME::StateMachineScene *scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit SemanticZoomManager(QObject *parent = nullptr);

    StateMachineScene *scene() const;
    void setScene(StateMachineScene *scene);

    RuntimeController *runtimeController() const;
    void setRuntimeController(RuntimeController *controller);

    bool isEnabled() const;
    void setEnabled(bool enabled);

private Q_SLOTS:
    void handleActiveConfigurationChanged(const QSet<KDSME::State *> &configuration);
    void updateRuntimeController();

Q_SIGNALS:
    void sceneChanged(KDSME::StateMachineScene *controller);
    void enabledChanged(bool enabled);

private:
    StateMachineScene *m_scene;
    RuntimeController *m_runtimeController;
    bool m_enabled;
};

}

#endif
