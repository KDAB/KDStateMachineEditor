/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#ifndef KDSME_RUNTIMECONTROLLER_H
#define KDSME_RUNTIMECONTROLLER_H

#include "kdsme_core_export.h"

#include <QObject>
#include <QRectF>
#include <QSet>

namespace KDSME {

class State;
class Transition;

class KDSME_CORE_EXPORT RuntimeController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSet<KDSME::State*> activeConfiguration READ activeConfiguration NOTIFY activeConfigurationChanged)
    Q_PROPERTY(KDSME::Transition* lastTransition READ lastTransition NOTIFY lastTransitionChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(QRectF activeRegion READ activeRegion NOTIFY activeRegionChanged)

public:
    typedef QSet<State*> Configuration;

    explicit RuntimeController(QObject* parent = nullptr);
    ~RuntimeController();

    Configuration activeConfiguration() const;
    QList<Configuration> lastConfigurations() const;
    void setActiveConfiguration(const Configuration& configuration);

    QList<Transition*> lastTransitions() const;
    Transition* lastTransition() const;
    void setLastTransition(Transition* transition);

    bool isRunning() const;
    void setIsRunning(bool isRunning);

    Q_INVOKABLE float activenessForState(KDSME::State* state) const;
    Q_INVOKABLE float activenessForTransition(KDSME::Transition* transition);

    int historySize() const;
    void setHistorySize(int size);

    QRectF activeRegion() const;

    void clear();

Q_SIGNALS:
    void activeConfigurationChanged(const QSet<State*>& configuration);
    void lastTransitionChanged(Transition* transition);
    void isRunningChanged(bool isRunning);
    void activeRegionChanged(const QRectF& region);

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

#endif
