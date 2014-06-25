/*
  propertyeditor.cpp

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

#include "propertyeditor.h"

#include "commandcontroller.h"
#include "command/modifypropertycommand.h"
#include "command/modifytransitioncommand.h"
#include "command/modifyinitialstatecommand.h"
#include "element.h"
#include "elementmodel.h"
#include "elementutil.h"
#include "ui_statepropertyeditor.h"
#include "ui_transitionpropertyeditor.h"

#include <QItemSelectionModel>
#include <QMetaProperty>

using namespace KDSME;

PropertyEditor::PropertyEditor(QWidget *parent)
    : QStackedWidget(parent)
    , m_selectionModel(nullptr)
    , m_commandController(nullptr)
{
    m_stateWidget = new Ui::StatePropertyEditor;
    m_transitionWidget = new Ui::TransitionPropertyEditor;
    m_noWidgetIndex = addWidget(new QWidget(this));

    QWidget* w = new QWidget(this);
    m_stateWidget->setupUi(w);
    m_stateWidgetIndex = addWidget(w);
    w = new QWidget(this);
    m_transitionWidget->setupUi(w);
    m_transitionWidgetIndex = addWidget(w);

    m_widgetToPropertyMap.insert(m_stateWidget->labelLineEdit, "label");
    m_widgetToPropertyMap.insert(m_stateWidget->onEntryEditor, "onEntry");
    m_widgetToPropertyMap.insert(m_stateWidget->onExitEditor, "onExit");
    m_widgetToPropertyMap.insert(m_stateWidget->childModeEdit, "childMode");
    m_widgetToPropertyMap.insert(m_stateWidget->historyTypeEdit, "historyType");
    m_widgetToPropertyMap.insert(m_transitionWidget->labelLineEdit, "label");
    m_widgetToPropertyMap.insert(m_transitionWidget->guardEditor, "guard");
    m_widgetToPropertyMap.insert(m_transitionWidget->signalEdit, "signal");
    m_widgetToPropertyMap.insert(m_transitionWidget->timeoutEdit, "timeout");

    connect(m_stateWidget->labelLineEdit, SIGNAL(editingFinished()), SLOT(updateSimpleProperty()));
    connect(m_stateWidget->initialStateComboBox, SIGNAL(activated(QString)), SLOT(setInitalState(QString)));
    connect(m_stateWidget->onEntryEditor, SIGNAL(editingFinished(QString)), SLOT(updateSimpleProperty()));
    connect(m_stateWidget->onExitEditor, SIGNAL(editingFinished(QString)), SLOT(updateSimpleProperty()));
    connect(m_stateWidget->childModeEdit, SIGNAL(currentIndexChanged(int)), SLOT(updateSimpleProperty()));
    connect(m_stateWidget->childModeEdit, SIGNAL(currentIndexChanged(int)), SLOT(childModeChanged()));
    connect(m_stateWidget->historyTypeEdit, SIGNAL(currentIndexChanged(int)), SLOT(updateSimpleProperty()));

    connect(m_transitionWidget->labelLineEdit, SIGNAL(editingFinished()), SLOT(updateSimpleProperty()));
    connect(m_transitionWidget->sourceStateComboBox, SIGNAL(activated(QString)), SLOT(setSourceState(QString)));
    connect(m_transitionWidget->targetStateComboBox, SIGNAL(activated(QString)), SLOT(setTargetState(QString)));
    connect(m_transitionWidget->guardEditor, SIGNAL(editingFinished(QString)), SLOT(updateSimpleProperty()));
    connect(m_transitionWidget->signalEdit, SIGNAL(editingFinished()), SLOT(updateSimpleProperty()));
    connect(m_transitionWidget->timeoutEdit, SIGNAL(valueChanged(int)), SLOT(updateSimpleProperty()));

    setCurrentIndex(m_noWidgetIndex);
}

PropertyEditor::~PropertyEditor()
{
    delete m_stateWidget;
    delete m_transitionWidget;
}

void PropertyEditor::setSelectionModel(QItemSelectionModel *selectionModel)
{
    if (m_selectionModel) {
        disconnect(m_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(currentChanged(QModelIndex,QModelIndex)));
    }

    m_selectionModel = selectionModel;

    if (m_selectionModel) {
        connect(m_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(currentChanged(QModelIndex,QModelIndex)));
    }
}

void PropertyEditor::setCommandController(CommandController* cmdController)
{
    m_commandController = cmdController;
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

void PropertyEditor::currentChanged(const QModelIndex &currentIndex, const QModelIndex &/*previous*/)
{
    if (m_currentElement)
        disconnect(m_currentElement, 0, this, SLOT(loadFromCurrentElement()));

    m_currentElement = currentIndex.data(StateModel::ElementRole).value<Element*>();
    loadFromCurrentElement();
    monitorElement(m_currentElement);
}

void PropertyEditor::monitorElement(KDSME::Element* element)
{
    if (!element)
        return;

    for (int i = 0; i < element->metaObject()->propertyCount(); ++i) {
        const QMetaProperty prop = element->metaObject()->property(i);
        if (!prop.hasNotifySignal())
            continue;
        connect(element, "2" + prop.notifySignal().methodSignature(), this, SLOT(loadFromCurrentElement()));
    }
}

void PropertyEditor::loadFromCurrentElement()
{
    State *state = current<State>();
    if (state && state->type() != Element::PseudoStateType) {
        m_stateWidget->labelLineEdit->setText(state->label());
        m_stateWidget->initialStateComboBox->clear();

        m_stateWidget->initialStateLabel->setVisible(state->isComposite());
        m_stateWidget->initialStateComboBox->setVisible(state->isComposite());
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

        m_stateWidget->onEntryEditor->setPlainText(state->onEntry());
        m_stateWidget->onExitEditor->setPlainText(state->onExit());

        HistoryState *historyState = current<HistoryState>();
        m_stateWidget->historyTypeLabel->setVisible(historyState);
        m_stateWidget->historyTypeEdit->setVisible(historyState);
        if (historyState)
            m_stateWidget->historyTypeEdit->setCurrentIndex(historyState->historyType());

        setCurrentIndex(m_stateWidgetIndex); // State page

    } else if (Transition *transition = current<Transition>()) {
        m_transitionWidget->labelLineEdit->setText(transition->label());

        m_transitionWidget->sourceStateComboBox->clear();
        State* sourceState = transition->sourceState();
        Q_ASSERT(sourceState);
        m_transitionWidget->sourceStateComboBox->addItems(allStates(sourceState->machine()));
        if (sourceState)
            m_transitionWidget->sourceStateComboBox->setCurrentText(sourceState->label());
        else
            m_transitionWidget->sourceStateComboBox->setCurrentText(QString());

        m_transitionWidget->targetStateComboBox->clear();
        State* targetState = transition->targetState();
        m_transitionWidget->targetStateComboBox->addItems(allStates(sourceState->machine()));
        if (targetState)
            m_transitionWidget->targetStateComboBox->setCurrentText(targetState->label());
        else
            m_transitionWidget->targetStateComboBox->setCurrentText(QString());

        m_transitionWidget->guardEditor->setPlainText(transition->guard());
        setCurrentIndex(m_transitionWidgetIndex); // Transition page

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
        setCurrentIndex(m_noWidgetIndex);
    }
}

void PropertyEditor::updateSimpleProperty()
{
    if (!sender() || !m_currentElement || !sender()->metaObject()->userProperty().isValid())
        return;

    const QString propertyName = m_widgetToPropertyMap.value(sender());
    Q_ASSERT(!propertyName.isEmpty());

    const QVariant currentValue = m_currentElement->property(propertyName.toUtf8());

    QVariant newValue;
    QComboBox *comboBox = qobject_cast<QComboBox*>(sender());
    if (comboBox && currentValue.type() != QVariant::String) { // the user property on QComboBox is currentString, not always what we want
        newValue = comboBox->currentIndex();
    } else {
        newValue = sender()->metaObject()->userProperty().read(sender());
    }
    if (currentValue == newValue)
        return;

    Q_ASSERT(m_commandController);
    ModifyPropertyCommand *command = new ModifyPropertyCommand(m_currentElement, propertyName.toUtf8(), newValue);
    m_commandController->undoStack()->push(command);
}

void PropertyEditor::setInitalState(const QString &label)
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

void PropertyEditor::setSourceState(const QString &label)
{
    Transition* transition = current<Transition>();
    Q_ASSERT(transition);
    if (transition) {
        State *sourceState = ElementUtil::findState(transition->sourceState()->machine(), label);
        if (transition->sourceState() != sourceState) {
            ModifyTransitionCommand *command = new ModifyTransitionCommand(transition);
            command->setSourceState(sourceState);
            m_commandController->undoStack()->push(command);
        }
    }
}

void PropertyEditor::setTargetState(const QString &label)
{
    Transition* transition = current<Transition>();
    Q_ASSERT(transition);
    if (transition) {
        State *targetState = ElementUtil::findState(transition->sourceState()->machine(), label);
        if (transition->targetState() != targetState) {
            ModifyTransitionCommand *command = new ModifyTransitionCommand(transition);
            command->setTargetState(targetState);
            m_commandController->undoStack()->push(command);
        }
    }
}

void PropertyEditor::childModeChanged()
{
    const bool parallelMode = m_stateWidget->childModeEdit->currentIndex() == State::ParallelStates;

    m_stateWidget->initialStateLabel->setEnabled(!parallelMode);
    m_stateWidget->initialStateComboBox->setEnabled(!parallelMode);
}
