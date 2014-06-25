/*
  element.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
  Author: Kevin Funk <kevin.funk@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef KDSME_MODEL_ELEMENT_H
#define KDSME_MODEL_ELEMENT_H

#include "kdsme_core_export.h"

#include <QObject>
#include <QAbstractState>
#include <QMetaType>

namespace KDSME {

class State;
class StateMachine;

class KDSME_CORE_EXPORT Element : public QObject
{
    Q_OBJECT
    Q_ENUMS(Flags)
    Q_PROPERTY(Type type READ type CONSTANT FINAL)
    Q_PROPERTY(Flags flags READ flags CONSTANT FINAL)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged FINAL)

public:
    enum Type {
        ElementType,

        // Transition types
        TransitionType,
        SignalTransitionType,
        TimeoutTransitionType,

        // State types
        StateType,
        StateMachineType,
        HistoryStateType,
        FinalStateType,
        PseudoStateType
    };
    Q_ENUMS(Type);

    enum Flag {
        NoFlags = 0x0,
        ElementIsEnabled = 0x1,
        ElementIsSelectable = 0x2,
        ElementIsEditable = 0x4,
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    explicit Element(QObject* parent = 0);
    virtual ~Element();

    virtual Type type() const;

    virtual Flags flags() const;

    /**
     * Label of this state (required to be unique)
     */
    QString label() const;
    void setLabel(const QString& label);

    /**
     * Internal ID (e.g. memory address)
     *
     * Internal id you can use to keep a mapping between elements and your data structures
     */
    quintptr internalId() const;
    void setInternalId(quintptr id);

    void setInternalPointer(void* ptr);
    void* internalPointer() const;

    Element* parentElement() const;
    QList<Element*> childElements() const;

    virtual QString toDisplayString() const;

    Q_INVOKABLE static const char* typeToString(Type type);
    Q_INVOKABLE static Type stringToType(const char* type);

Q_SIGNALS:
    void labelChanged(const QString& label);

private:
    struct Private;
    QScopedPointer<Private> d;
};

class KDSME_CORE_EXPORT Transition : public Element
{
    Q_OBJECT
    Q_PROPERTY(KDSME::State* sourceState READ sourceState WRITE setSourceState NOTIFY sourceStateChanged FINAL)
    Q_PROPERTY(KDSME::State* targetState READ targetState WRITE setTargetState NOTIFY targetStateChanged FINAL)
    Q_PROPERTY(QString guard READ guard WRITE setGuard NOTIFY guardChanged FINAL)

public:
    explicit Transition(State* sourceState = 0);
    virtual ~Transition();

    virtual Type type() const;

    Q_INVOKABLE KDSME::StateMachine* machine() const;

    State* sourceState() const;
    void setSourceState(State* sourceState);

    State* targetState() const;
    void setTargetState(State* targetState);

    /**
     * The guard condition for this transition (executable content)
     *
     * @return Any boolean expression in the current language
     */
    QString guard() const;
    void setGuard(const QString& guard);

Q_SIGNALS:
    void sourceStateChanged(State* sourceState);
    void targetStateChanged(State* targetState);
    void guardChanged(const QString& guard);

private:
    struct Private;
    QScopedPointer<Private> d;
};

KDSME_CORE_EXPORT QDebug operator<<(QDebug dbg, const Transition* transition);

class KDSME_CORE_EXPORT State : public Element
{
    Q_OBJECT
    Q_ENUMS(ChildMode)
    Q_PROPERTY(QString onEntry READ onEntry WRITE setOnEntry NOTIFY onEntryChanged FINAL)
    Q_PROPERTY(QString onExit READ onExit WRITE setOnExit NOTIFY onExitChanged FINAL)
    Q_PROPERTY(ChildMode childMode READ childMode WRITE setChildMode NOTIFY childModeChanged FINAL)
    Q_PROPERTY(bool isComposite READ isComposite NOTIFY isCompositeChanged FINAL)

public:
    enum ChildMode {
        ExclusiveStates,
        ParallelStates,
    };

    explicit State(State* parent = 0);
    ~State();

    virtual Type type() const Q_DECL_OVERRIDE;

    State* parentState() const;

    State* initialState() const;
    void setInitialState(State* initialState);

    QList<State*> childStates() const;

    QList<Transition*> transitions() const;
    void addTransition(Transition* transition);
    Transition* addTransition(State* target);

    /**
     * Executable content to be executed when the state is entered
     */
    QString onEntry() const;
    void setOnEntry(const QString& onEntry);

    /**
     * Executable content to be executed when the state is exited.
     */
    QString onExit() const;
    void setOnExit(const QString& onExit);

    ChildMode childMode() const;
    void setChildMode(ChildMode childMode);

    /**
     * @return True in case this state contains other states, otherwise false
     */
    bool isComposite() const;

    Q_INVOKABLE KDSME::StateMachine* machine() const;

protected:
    virtual bool event(QEvent* event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void onEntryChanged(const QString& onEntry);
    void onExitChanged(const QString& onExit);
    void childModeChanged(ChildMode childMode);
    void isCompositeChanged(bool isComposite);

private:
    struct Private;
    QScopedPointer<Private> d;
};

class KDSME_CORE_EXPORT HistoryState : public State
{
    Q_OBJECT
    Q_ENUMS(HistoryType)
    Q_PROPERTY(KDSME::State* defaultState READ defaultState WRITE setDefaultState NOTIFY defaultStateChanged FINAL)
    Q_PROPERTY(HistoryType historyType READ historyType WRITE setHistoryType NOTIFY historyTypeChanged FINAL)

public:
    enum HistoryType {
        ShallowHistory,
        DeepHistory
    };

    explicit HistoryState(State* parent = 0);
    virtual ~HistoryState();

    virtual Type type() const Q_DECL_OVERRIDE;
    virtual QString toDisplayString() const Q_DECL_OVERRIDE;

    State* defaultState() const;
    void setDefaultState(State *state);

    HistoryType historyType() const;
    void setHistoryType(HistoryType historyType);

Q_SIGNALS:
    void defaultStateChanged(State *state);
    void historyTypeChanged();

private:
    struct Private;
    QScopedPointer<Private> d;
};

KDSME_CORE_EXPORT QDebug operator<<(QDebug dbg, const State* representative);

class KDSME_CORE_EXPORT FinalState : public State
{
    Q_OBJECT

public:
    explicit FinalState(State* parent = 0);

    virtual Type type() const Q_DECL_OVERRIDE;
};

class KDSME_CORE_EXPORT StateMachine : public State
{
    Q_OBJECT

public:
    explicit StateMachine(QObject* parent = 0);
    virtual ~StateMachine();

    virtual Type type() const Q_DECL_OVERRIDE;
};

class KDSME_CORE_EXPORT PseudoState : public State
{
    Q_OBJECT
    Q_PROPERTY(Kind kind READ kind WRITE setKind NOTIFY kindChanged FINAL)

public:
    enum Kind {
        InitialState
    };
    Q_ENUMS(Kind);

    explicit PseudoState(Kind kind = InitialState, State* parent = 0);
    virtual ~PseudoState();

    virtual Type type() const Q_DECL_OVERRIDE;

    virtual Flags flags() const Q_DECL_OVERRIDE;

    Kind kind() const;
    void setKind(Kind kind);

    QString kindString() const;

    virtual QString toDisplayString() const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void kindChanged(Kind kind);

private:
    struct Private;
    QScopedPointer<Private> d;
};

class KDSME_CORE_EXPORT SignalTransition : public Transition
{
    Q_OBJECT
    Q_PROPERTY(QString signal READ signal WRITE setSignal NOTIFY signalChanged)

public:
    explicit SignalTransition(State* sourceState);
    virtual ~SignalTransition();

    virtual Type type() const Q_DECL_OVERRIDE;

    QString signal() const;
    void setSignal(const QString& signal);

Q_SIGNALS:
    void signalChanged(const QString& signal);

private:
    struct Private;
    QScopedPointer<Private> d;
};

class KDSME_CORE_EXPORT TimeoutTransition : public Transition
{
    Q_OBJECT
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)

public:
    explicit TimeoutTransition(State* sourceState);
    virtual ~TimeoutTransition();

    virtual Type type() const Q_DECL_OVERRIDE;

    int timeout() const;
    void setTimeout(int timeout);

Q_SIGNALS:
    void timeoutChanged(int timeout);

public:
    struct Private;
    QScopedPointer<Private> d;
};

KDSME_CORE_EXPORT QDebug operator<<(QDebug dbg, const PseudoState* representative);

}

Q_DECLARE_METATYPE(KDSME::Element::Type);
Q_DECLARE_METATYPE(KDSME::Element*);
Q_DECLARE_METATYPE(KDSME::PseudoState::Kind);

#endif // TREESTRUCTURE_H
