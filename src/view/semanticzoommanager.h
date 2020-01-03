/*
  semanticzoommanager.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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
    Q_PROPERTY(KDSME::StateMachineScene* scene READ scene WRITE setScene NOTIFY sceneChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

public:
    explicit SemanticZoomManager(QObject* parent = nullptr);

    StateMachineScene* scene() const;
    void setScene(StateMachineScene* scene);

    RuntimeController* runtimeController() const;
    void setRuntimeController(RuntimeController* controller);

    bool isEnabled() const;
    void setEnabled(bool enabled);

private Q_SLOTS:
    void handleActiveConfigurationChanged(const QSet<State*>& configuration);
    void updateRuntimeController();

Q_SIGNALS:
    void sceneChanged(StateMachineScene* controller);
    void enabledChanged(bool enabled);

private:
    StateMachineScene* m_scene;
    RuntimeController* m_runtimeController;
    bool m_enabled;
};

}

#endif
