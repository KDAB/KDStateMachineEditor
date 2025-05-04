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

#ifndef KDSME_MODEL_STATE_H
#define KDSME_MODEL_STATE_H

#include "element.h"

namespace KDSME {

class RuntimeController;
class Transition;
class SignalTransition;
class TimeoutTransition;

class KDSME_CORE_EXPORT State : public Element
{
    Q_OBJECT
    Q_PROPERTY(QString onEntry READ onEntry WRITE setOnEntry NOTIFY onEntryChanged FINAL)
    Q_PROPERTY(QString onExit READ onExit WRITE setOnExit NOTIFY onExitChanged FINAL)
    Q_PROPERTY(ChildMode childMode READ childMode WRITE setChildMode NOTIFY childModeChanged FINAL)
    Q_PROPERTY(bool isComposite READ isComposite NOTIFY isCompositeChanged FINAL)
    Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded NOTIFY expandedChanged FINAL)

public:
    enum ChildMode
    {
        ExclusiveStates,
        ParallelStates,
    };
    Q_ENUM(ChildMode)

    explicit State(State *parent = nullptr);
    ~State();

    Type type() const override;

    Q_INVOKABLE KDSME::State *parentState() const;

    State *initialState() const;
    void setInitialState(State *initialState);

    QList<State *> childStates() const;

    QList<Transition *> transitions() const;
    void addTransition(Transition *transition);
    SignalTransition *addSignalTransition(State *target, const QString &silgnal = QString());
    TimeoutTransition *addTimeoutTransition(State *target, int timeout);

    /**
     * Executable content to be executed when the state is entered
     */
    QString onEntry() const;
    void setOnEntry(const QString &onEntry);

    /**
     * Executable content to be executed when the state is exited.
     */
    QString onExit() const;
    void setOnExit(const QString &onExit);

    ChildMode childMode() const;
    void setChildMode(ChildMode childMode);

    /**
     * @return True in case this state contains other states, otherwise false
     */
    bool isComposite() const;

    bool isExpanded() const;
    void setExpanded(bool expanded);

    Q_INVOKABLE KDSME::StateMachine *machine() const;

protected:
    bool event(QEvent *event) override;

Q_SIGNALS:
    void onEntryChanged(const QString &onEntry);
    void onExitChanged(const QString &onExit);
    void childModeChanged(KDSME::State::ChildMode childMode);
    void isCompositeChanged(bool isComposite);
    void expandedChanged(bool expanded);

private:
    struct Private;
    QScopedPointer<Private> d;
};

class KDSME_CORE_EXPORT HistoryState : public State
{
    Q_OBJECT
    Q_PROPERTY(KDSME::State *defaultState READ defaultState WRITE setDefaultState NOTIFY defaultStateChanged FINAL)
    Q_PROPERTY(HistoryType historyType READ historyType WRITE setHistoryType NOTIFY historyTypeChanged FINAL)

public:
    enum HistoryType
    {
        ShallowHistory,
        DeepHistory
    };
    Q_ENUM(HistoryType)

    explicit HistoryState(State *parent = nullptr);
    explicit HistoryState(HistoryType type, State *parent = nullptr);
    ~HistoryState();

    Type type() const override;
    QString toDisplayString() const override;

    State *defaultState() const;
    void setDefaultState(State *state);

    HistoryType historyType() const;
    void setHistoryType(HistoryType historyType);

Q_SIGNALS:
    void defaultStateChanged(KDSME::State *state);
    void historyTypeChanged();

private:
    struct Private;
    QScopedPointer<Private> d;
};

KDSME_CORE_EXPORT QDebug operator<<(QDebug dbg, const State *state);

class KDSME_CORE_EXPORT FinalState : public State
{
    Q_OBJECT

public:
    explicit FinalState(State *parent = nullptr);
    ~FinalState();

    Type type() const override;

private:
    struct Private;
    QScopedPointer<Private> d;
};

class KDSME_CORE_EXPORT StateMachine : public State
{
    Q_OBJECT
    Q_PROPERTY(KDSME::RuntimeController *runtimeController READ runtimeController WRITE setRuntimeController NOTIFY runtimeControllerChanged)

public:
    explicit StateMachine(QObject *parent = nullptr);
    ~StateMachine();

    Type type() const override;

    RuntimeController *runtimeController() const;
    void setRuntimeController(RuntimeController *runtimeController);

Q_SIGNALS:
    void runtimeControllerChanged(KDSME::RuntimeController *runtimeController);

private:
    struct Private;
    QScopedPointer<Private> d;
};

class KDSME_CORE_EXPORT PseudoState : public State
{
    Q_OBJECT
    Q_PROPERTY(Kind kind READ kind WRITE setKind NOTIFY kindChanged FINAL)

public:
    enum Kind
    {
        InitialState
    };
    Q_ENUM(Kind)

    explicit PseudoState(Kind kind = InitialState, State *parent = nullptr);
    ~PseudoState();

    Type type() const override;

    Kind kind() const;
    void setKind(Kind kind);

    QString kindString() const;

    QString toDisplayString() const override;

Q_SIGNALS:
    void kindChanged(KDSME::PseudoState::Kind kind);

private:
    struct Private;
    QScopedPointer<Private> d;
};

KDSME_CORE_EXPORT QDebug operator<<(QDebug dbg, const PseudoState *state);

}

Q_DECLARE_METATYPE(KDSME::PseudoState::Kind)

#endif
