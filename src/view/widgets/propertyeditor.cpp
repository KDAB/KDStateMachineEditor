/*
  propertyeditor.cpp

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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

#include "propertyeditor.h"

#include "commandcontroller.h"
#include "command/modifypropertycommand_p.h"
#include "command/modifytransitioncommand_p.h"
#include "command/modifyinitialstatecommand_p.h"
#include "command/modifydefaultstatecommand_p.h"
#include "state.h"
#include "transition.h"
#include "elementmodel.h"
#include "elementutil.h"
#include "ui_statepropertyeditor.h"
#include "ui_transitionpropertyeditor.h"

#include <QDebug>
#include <QHash>
#include <QItemSelectionModel>
#include <QMetaProperty>

using namespace KDSME;

struct PropertyEditor::Private
{
    Private(PropertyEditor* q);

    template <typename T> inline T* current() const
    {
        return qobject_cast<T*>(m_currentElement);
    }

    void setCurrentElement(KDSME::Element* element);

    // slots
    void updateSimpleProperty();
    void setInitalState(const QString& label);
    void setDefaultState(const QString& label);
    void setSourceState(const QString& label);
    void setTargetState(const QString& label);
    void childModeChanged();
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void modelAboutToBeReset();
    void loadFromCurrentElement();

    PropertyEditor* q;
    QItemSelectionModel* m_selectionModel;
    CommandController *m_commandController;
    KDSME::StateModel *m_stateModel;
    QPointer<KDSME::Element> m_currentElement;
    Ui::StatePropertyEditor* m_stateWidget;
    Ui::TransitionPropertyEditor* m_transitionWidget;
    int m_noWidgetIndex, m_stateWidgetIndex, m_transitionWidgetIndex;

    QHash<QObject*, QString> m_widgetToPropertyMap;
};

PropertyEditor::Private::Private(PropertyEditor* q)
    : q(q)
    , m_selectionModel(nullptr)
    , m_commandController(nullptr)
    , m_stateModel(nullptr)
    , m_stateWidget(nullptr)
    , m_transitionWidget(nullptr)
    , m_noWidgetIndex(-1)
    , m_stateWidgetIndex(-1)
    , m_transitionWidgetIndex(-1)
{
}

PropertyEditor::PropertyEditor(QWidget *parent)
    : QStackedWidget(parent)
    , d(new Private(this))
{
    d->m_stateWidget = new Ui::StatePropertyEditor;
    d->m_transitionWidget = new Ui::TransitionPropertyEditor;
    d->m_noWidgetIndex = addWidget(new QWidget(this));

    QWidget* w = new QWidget(this);
    d->m_stateWidget->setupUi(w);
    d->m_stateWidgetIndex = addWidget(w);
    w = new QWidget(this);
    d->m_transitionWidget->setupUi(w);
    d->m_transitionWidgetIndex = addWidget(w);

    d->m_widgetToPropertyMap.insert(d->m_stateWidget->labelLineEdit, "label");
    d->m_widgetToPropertyMap.insert(d->m_stateWidget->onEntryEditor, "onEntry");
    d->m_widgetToPropertyMap.insert(d->m_stateWidget->onExitEditor, "onExit");
    d->m_widgetToPropertyMap.insert(d->m_stateWidget->childModeEdit, "childMode");
    d->m_widgetToPropertyMap.insert(d->m_stateWidget->historyTypeEdit, "historyType");
    d->m_widgetToPropertyMap.insert(d->m_transitionWidget->labelLineEdit, "label");
    d->m_widgetToPropertyMap.insert(d->m_transitionWidget->guardEditor, "guard");
    d->m_widgetToPropertyMap.insert(d->m_transitionWidget->signalEdit, "signal");
    d->m_widgetToPropertyMap.insert(d->m_transitionWidget->timeoutEdit, "timeout");

    connect(d->m_stateWidget->labelLineEdit, SIGNAL(editingFinished()), SLOT(updateSimpleProperty()));
    connect(d->m_stateWidget->initialStateComboBox, SIGNAL(activated(QString)), SLOT(setInitalState(QString)));
    connect(d->m_stateWidget->defaultStateComboBox, SIGNAL(activated(QString)), SLOT(setDefaultState(QString)));
    connect(d->m_stateWidget->onEntryEditor, SIGNAL(editingFinished(QString)), SLOT(updateSimpleProperty()));
    connect(d->m_stateWidget->onExitEditor, SIGNAL(editingFinished(QString)), SLOT(updateSimpleProperty()));
    connect(d->m_stateWidget->childModeEdit, SIGNAL(currentIndexChanged(int)), SLOT(updateSimpleProperty()));
    connect(d->m_stateWidget->childModeEdit, SIGNAL(currentIndexChanged(int)), SLOT(childModeChanged()));
    connect(d->m_stateWidget->historyTypeEdit, SIGNAL(currentIndexChanged(int)), SLOT(updateSimpleProperty()));

    connect(d->m_transitionWidget->labelLineEdit, SIGNAL(editingFinished()), SLOT(updateSimpleProperty()));
    connect(d->m_transitionWidget->sourceStateComboBox, SIGNAL(activated(QString)), SLOT(setSourceState(QString)));
    connect(d->m_transitionWidget->targetStateComboBox, SIGNAL(activated(QString)), SLOT(setTargetState(QString)));
    connect(d->m_transitionWidget->guardEditor, SIGNAL(editingFinished(QString)), SLOT(updateSimpleProperty()));
    connect(d->m_transitionWidget->signalEdit, SIGNAL(editingFinished()), SLOT(updateSimpleProperty()));
    connect(d->m_transitionWidget->timeoutEdit, SIGNAL(valueChanged(int)), SLOT(updateSimpleProperty()));

    setCurrentIndex(d->m_noWidgetIndex);
}

PropertyEditor::~PropertyEditor()
{
    delete d->m_stateWidget;
    delete d->m_transitionWidget;
}

void PropertyEditor::setSelectionModel(QItemSelectionModel *selectionModel)
{
    if (d->m_selectionModel) {
        disconnect(d->m_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex,QModelIndex)));
        disconnect(d->m_selectionModel->model(), SIGNAL(modelAboutToBeReset()),
                   this, SLOT(modelAboutToBeReset()));

    }

    d->m_selectionModel = selectionModel;

    if (d->m_selectionModel) {
        connect(d->m_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(currentChanged(QModelIndex,QModelIndex)));
        connect(d->m_selectionModel->model(), SIGNAL(modelAboutToBeReset()),
                this, SLOT(modelAboutToBeReset()));
    }
}

void PropertyEditor::setCommandController(CommandController* cmdController)
{
    d->m_commandController = cmdController;
}

void PropertyEditor::setStateModel(StateModel *selectionModel)
{
    d->m_stateModel = selectionModel;
}

static QStringList allStates(const State * state)
{
    QStringList ret;
    if (!state)
        return ret;

    if (!state->label().isEmpty())
        ret << state->label();
    foreach (const State * st, state->childStates())
        ret << allStates(st);
    ret.removeDuplicates();
    return ret;
}

static QStringList childStates(const State * state)
{
    QStringList ret;
    ret << QString();
    if (!state)
        return ret;

    foreach (const State * st, state->childStates())
        if (!st->label().isEmpty())
            ret << st->label();

    ret.removeDuplicates();
    ret.sort();
    return ret;
}

void PropertyEditor::Private::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    KDSME::Element* currentElement = current.data(StateModel::ElementRole).value<Element*>();
    setCurrentElement(currentElement);
}

void PropertyEditor::Private::modelAboutToBeReset()
{
    setCurrentElement(nullptr);
}

void PropertyEditor::Private::setCurrentElement(KDSME::Element* element)
{
    if (m_currentElement == element) {
        return;
    }

    if (m_currentElement) {
        q->disconnect(m_currentElement, nullptr, q, SLOT(loadFromCurrentElement()));
    }

    m_currentElement = element;

    if (m_currentElement) {
        for (int i = 0; i < m_currentElement->metaObject()->propertyCount(); ++i) {
            const QMetaProperty prop = m_currentElement->metaObject()->property(i);
            if (!prop.hasNotifySignal())
                continue;
            q->connect(m_currentElement, "2" + prop.notifySignal().methodSignature(), q, SLOT(loadFromCurrentElement())); //krazy:exclude=doublequote_chars
        }
    }
    loadFromCurrentElement();
}

void PropertyEditor::Private::loadFromCurrentElement()
{
    State *state = current<State>();
    if (state && state->flags().testFlag(Element::ElementIsEditable)) {
        m_stateWidget->labelLineEdit->setText(state->label());
        m_stateWidget->initialStateComboBox->clear();
        m_stateWidget->defaultStateComboBox->clear();

        m_stateWidget->initialStateLabel->setVisible(state->isComposite());
        m_stateWidget->initialStateComboBox->setVisible(state->isComposite());
        m_stateWidget->defaultStateLabel->setVisible(state->type() == Element::HistoryStateType);
        m_stateWidget->defaultStateComboBox->setVisible(state->type() == Element::HistoryStateType);
        m_stateWidget->childModeLabel->setVisible(state->isComposite());
        m_stateWidget->childModeEdit->setVisible(state->isComposite());

        if (state->isComposite()) {
            m_stateWidget->initialStateComboBox->addItems(childStates(state));
            State* initialState = ElementUtil::findInitialState(state);
            if (initialState)
                m_stateWidget->initialStateComboBox->setCurrentText(initialState->label());
            else
                m_stateWidget->initialStateComboBox->setCurrentIndex(0);

            m_stateWidget->childModeEdit->setCurrentIndex(state->childMode());
        }

        if (state->type() == Element::HistoryStateType) {
            m_stateWidget->defaultStateComboBox->addItems(allStates(state->machine()));
            State *defaultState = qobject_cast<HistoryState*>(state)->defaultState();
            m_stateWidget->defaultStateComboBox->setCurrentText(defaultState ? defaultState->label() : "");
        }

        m_stateWidget->onEntryEditor->setPlainText(state->onEntry());
        m_stateWidget->onExitEditor->setPlainText(state->onExit());

        HistoryState *historyState = current<HistoryState>();
        m_stateWidget->historyTypeLabel->setVisible(historyState);
        m_stateWidget->historyTypeEdit->setVisible(historyState);
        if (historyState)
            m_stateWidget->historyTypeEdit->setCurrentIndex(historyState->historyType());

        q->setCurrentIndex(m_stateWidgetIndex); // State page

    } else if (Transition *transition = current<Transition>()) {
        m_transitionWidget->labelLineEdit->setText(transition->label());

        m_transitionWidget->sourceStateComboBox->clear();
        State* sourceState = transition->sourceState();
        Q_ASSERT(sourceState);
        if (sourceState) {
            m_transitionWidget->sourceStateComboBox->addItems(allStates(sourceState->machine()));
            m_transitionWidget->sourceStateComboBox->setCurrentText(sourceState->label());
        } else {
            m_transitionWidget->sourceStateComboBox->setCurrentText(QString());
        }
        m_transitionWidget->targetStateComboBox->clear();
        State* targetState = transition->targetState();
        m_transitionWidget->targetStateComboBox->addItems(allStates(sourceState->machine()));
        if (targetState)
            m_transitionWidget->targetStateComboBox->setCurrentText(targetState->label());
        else
            m_transitionWidget->targetStateComboBox->setCurrentText(QString());

        m_transitionWidget->guardEditor->setPlainText(transition->guard());
        q->setCurrentIndex(m_transitionWidgetIndex); // Transition page

        SignalTransition *signalTransition = current<SignalTransition>();
        m_transitionWidget->signalLabel->setVisible(signalTransition);
        m_transitionWidget->signalEdit->setVisible(signalTransition);
        if (signalTransition)
            m_transitionWidget->signalEdit->setText(signalTransition->signal());

        TimeoutTransition *timeoutTransition = current<TimeoutTransition>();
        m_transitionWidget->timeoutLabel->setVisible(timeoutTransition);
        m_transitionWidget->timeoutEdit->setVisible(timeoutTransition);
        if (timeoutTransition)
            m_transitionWidget->timeoutEdit->setValue(timeoutTransition->timeout());

    } else {
        q->setCurrentIndex(m_noWidgetIndex);
    }
}

void PropertyEditor::Private::updateSimpleProperty()
{
    QObject* object = q->sender();
    if (!object || !m_currentElement || !object->metaObject()->userProperty().isValid())
        return;

    const QString propertyName = m_widgetToPropertyMap.value(object);
    Q_ASSERT(!propertyName.isEmpty());

    const QVariant currentValue = m_currentElement->property(propertyName.toUtf8());

    QVariant newValue;
    QComboBox *comboBox = qobject_cast<QComboBox*>(object);
    if (comboBox && currentValue.type() != QVariant::String) { // the user property on QComboBox is currentString, not always what we want
        newValue = comboBox->currentIndex();
    } else {
        newValue = object->metaObject()->userProperty().read(object);
    }
    if (currentValue == newValue)
        return;

    Q_ASSERT(m_commandController);
    ModifyPropertyCommand *command = new ModifyPropertyCommand(m_currentElement, propertyName.toUtf8(), newValue);
    m_commandController->undoStack()->push(command);
}

void PropertyEditor::Private::setInitalState(const QString &label)
{
    State* state = current<State>();
    Q_ASSERT(state);
    if (state) {
        State *currentInitialState = ElementUtil::findInitialState(state);
        State* initialState = ElementUtil::findState(state, label);
        if (currentInitialState != initialState) {
            ModifyInitialStateCommand *command = new ModifyInitialStateCommand(state, initialState);
            m_commandController->undoStack()->push(command);
        }
    }
}

void PropertyEditor::Private::setDefaultState(const QString& label)
{
    HistoryState* state = current<HistoryState>();
    Q_ASSERT(state);
    if (state) {
        State* defaultState = ElementUtil::findState(state->machine(), label);
        if (state->defaultState() != defaultState) {
            ModifyDefaultStateCommand *command = new ModifyDefaultStateCommand(state, defaultState);
            m_commandController->undoStack()->push(command);
        }
    }
}

void PropertyEditor::Private::setSourceState(const QString &label)
{
    Transition* transition = current<Transition>();
    if (transition) {
        State *sourceState = ElementUtil::findState(transition->sourceState()->machine(), label);
        if (transition->sourceState() != sourceState) {
            ModifyTransitionCommand *command = new ModifyTransitionCommand(transition, m_stateModel);
            command->setSourceState(sourceState);
            m_commandController->undoStack()->push(command);
        }
    }
}

void PropertyEditor::Private::setTargetState(const QString &label)
{
    Transition* transition = current<Transition>();
    if (transition) {
        State *targetState = ElementUtil::findState(transition->sourceState()->machine(), label);
        if (transition->targetState() != targetState) {
            ModifyTransitionCommand *command = new ModifyTransitionCommand(transition, m_stateModel);
            command->setTargetState(targetState);
            m_commandController->undoStack()->push(command);
        }
    }
}

void PropertyEditor::Private::childModeChanged()
{
    const bool parallelMode = m_stateWidget->childModeEdit->currentIndex() == State::ParallelStates;

    m_stateWidget->initialStateLabel->setEnabled(!parallelMode);
    m_stateWidget->initialStateComboBox->setEnabled(!parallelMode);
}

#include "moc_propertyeditor.cpp"
