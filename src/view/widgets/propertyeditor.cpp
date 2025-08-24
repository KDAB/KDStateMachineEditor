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

namespace {

QStringList allStates(const State *state)
{
    QStringList ret;
    if (!state)
        return ret;

    if (!state->label().isEmpty())
        ret << state->label();
    const auto childStates = state->childStates();
    for (const State *st : childStates)
        ret << allStates(st);
    ret.removeDuplicates();
    return ret;
}

QStringList childStates(const State *state)
{
    QStringList ret;
    ret << QString();
    if (!state)
        return ret;

    const auto childStates = state->childStates();
    for (const State *st : childStates) {
        if (!st->label().isEmpty()) {
            ret << st->label();
        }
    }

    ret.removeDuplicates();
    ret.sort();
    return ret;
}

}

struct PropertyEditor::Private // NOLINT(clang-analyzer-cplusplus.NewDelete)
{
    Private(PropertyEditor *q);

    template<typename T>
    [[nodiscard]] T *current() const
    {
        return qobject_cast<T *>(m_currentElement);
    }

    void setCurrentElement(KDSME::Element *element);

    // slots
    void updateSimpleProperty() const;
    void setInitalState(const QString &label) const;
    void setDefaultState(const QString &label) const;
    void setSourceState(const QString &label) const;
    void setTargetState(const QString &label) const;
    void childModeChanged() const;
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void modelAboutToBeReset();
    void loadFromCurrentElement() const;

    PropertyEditor *q;
    QItemSelectionModel *m_selectionModel;
    CommandController *m_commandController;
    KDSME::StateModel *m_stateModel;
    QPointer<KDSME::Element> m_currentElement;
    Ui::StatePropertyEditor *m_stateWidget;
    Ui::TransitionPropertyEditor *m_transitionWidget;
    int m_noWidgetIndex, m_stateWidgetIndex, m_transitionWidgetIndex;

    QHash<QObject *, QString> m_widgetToPropertyMap;
};

PropertyEditor::Private::Private(PropertyEditor *q)
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

    auto *w = new QWidget(this);
    d->m_stateWidget->setupUi(w);
    d->m_stateWidgetIndex = addWidget(w);
    w = new QWidget(this);
    d->m_transitionWidget->setupUi(w);
    d->m_transitionWidgetIndex = addWidget(w);

    d->m_widgetToPropertyMap.insert(d->m_stateWidget->labelLineEdit, QStringLiteral("label"));
    d->m_widgetToPropertyMap.insert(d->m_stateWidget->onEntryEditor, QStringLiteral("onEntry"));
    d->m_widgetToPropertyMap.insert(d->m_stateWidget->onExitEditor, QStringLiteral("onExit"));
    d->m_widgetToPropertyMap.insert(d->m_stateWidget->childModeEdit, QStringLiteral("childMode"));
    d->m_widgetToPropertyMap.insert(d->m_stateWidget->historyTypeEdit, QStringLiteral("historyType"));
    d->m_widgetToPropertyMap.insert(d->m_transitionWidget->labelLineEdit, QStringLiteral("label"));
    d->m_widgetToPropertyMap.insert(d->m_transitionWidget->guardEditor, QStringLiteral("guard"));
    d->m_widgetToPropertyMap.insert(d->m_transitionWidget->signalEdit, QStringLiteral("signal"));
    d->m_widgetToPropertyMap.insert(d->m_transitionWidget->timeoutEdit, QStringLiteral("timeout"));

    connect(d->m_stateWidget->labelLineEdit, SIGNAL(editingFinished()), this, SLOT(updateSimpleProperty())); // clazy:exclude=old-style-connect
    connect(d->m_stateWidget->initialStateComboBox, &QComboBox::textActivated, this, [this](const QString &text) { d->setInitalState(text); });
    connect(d->m_stateWidget->defaultStateComboBox, &QComboBox::textActivated, this, [this](const QString &text) { d->setDefaultState(text); });
    connect(d->m_stateWidget->onEntryEditor, SIGNAL(editingFinished(QString)), SLOT(updateSimpleProperty())); // clazy:exclude=old-style-connect
    connect(d->m_stateWidget->onExitEditor, SIGNAL(editingFinished(QString)), SLOT(updateSimpleProperty())); // clazy:exclude=old-style-connect
    connect(d->m_stateWidget->childModeEdit, SIGNAL(currentIndexChanged(int)), SLOT(updateSimpleProperty())); // clazy:exclude=old-style-connect
    connect(d->m_stateWidget->childModeEdit, SIGNAL(currentIndexChanged(int)), SLOT(childModeChanged())); // clazy:exclude=old-style-connect
    connect(d->m_stateWidget->historyTypeEdit, SIGNAL(currentIndexChanged(int)), SLOT(updateSimpleProperty())); // clazy:exclude=old-style-connect

    connect(d->m_transitionWidget->labelLineEdit, SIGNAL(editingFinished()), SLOT(updateSimpleProperty())); // clazy:exclude=old-style-connect
    connect(d->m_transitionWidget->sourceStateComboBox, &QComboBox::textActivated, this, [this](const QString &text) { d->setSourceState(text); });
    connect(d->m_transitionWidget->targetStateComboBox, &QComboBox::textActivated, this, [this](const QString &text) { d->setTargetState(text); });
    connect(d->m_transitionWidget->guardEditor, SIGNAL(editingFinished(QString)), SLOT(updateSimpleProperty())); // clazy:exclude=old-style-connect
    connect(d->m_transitionWidget->signalEdit, SIGNAL(editingFinished()), SLOT(updateSimpleProperty())); // clazy:exclude=old-style-connect
    connect(d->m_transitionWidget->timeoutEdit, SIGNAL(valueChanged(int)), SLOT(updateSimpleProperty())); // clazy:exclude=old-style-connect

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
        // clang-format off
        disconnect(d->m_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), // clazy:exclude=old-style-connect
                   this, SLOT(currentChanged(QModelIndex,QModelIndex)));
        disconnect(d->m_selectionModel->model(), SIGNAL(modelAboutToBeReset()), // clazy:exclude=old-style-connect
                   this, SLOT(modelAboutToBeReset()));
        // clang-format on
    }

    d->m_selectionModel = selectionModel;

    if (d->m_selectionModel) {
        // clang-format off
        connect(d->m_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), // clazy:exclude=old-style-connect
                this, SLOT(currentChanged(QModelIndex,QModelIndex)));
        connect(d->m_selectionModel->model(), SIGNAL(modelAboutToBeReset()), // clazy:exclude=old-style-connect
                this, SLOT(modelAboutToBeReset()));
        // clang-format on
    }
}

void PropertyEditor::setCommandController(CommandController *cmdController)
{
    d->m_commandController = cmdController;
}

void PropertyEditor::setStateModel(StateModel *selectionModel)
{
    d->m_stateModel = selectionModel;
}

void PropertyEditor::Private::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    auto *currentElement = current.data(StateModel::ElementRole).value<Element *>();
    setCurrentElement(currentElement);
}

void PropertyEditor::Private::modelAboutToBeReset()
{
    setCurrentElement(nullptr);
}

void PropertyEditor::Private::setCurrentElement(KDSME::Element *element)
{
    if (m_currentElement == element) {
        return;
    }

    if (m_currentElement) {
        QObject::disconnect(m_currentElement, nullptr, q, SLOT(loadFromCurrentElement()));
    }

    m_currentElement = element;

    if (m_currentElement) {
        for (int i = 0; i < m_currentElement->metaObject()->propertyCount(); ++i) {
            const QMetaProperty prop = m_currentElement->metaObject()->property(i);
            if (!prop.hasNotifySignal())
                continue;
            QObject::connect(m_currentElement, QByteArray { "2" + prop.notifySignal().methodSignature() }.constData(), q, SLOT(loadFromCurrentElement())); // krazy:exclude=doublequote_chars
        }
    }
    loadFromCurrentElement();
}

void PropertyEditor::Private::loadFromCurrentElement() const // NOLINT(readability-function-cognitive-complexity)
{
    auto *state = current<State>();
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
            State *initialState = ElementUtil::findInitialState(state);
            if (initialState)
                m_stateWidget->initialStateComboBox->setCurrentText(initialState->label());
            else
                m_stateWidget->initialStateComboBox->setCurrentIndex(0);

            m_stateWidget->childModeEdit->setCurrentIndex(state->childMode());
        }

        if (state->type() == Element::HistoryStateType) {
            m_stateWidget->defaultStateComboBox->addItems(allStates(state->machine()));
            State *defaultState = qobject_cast<HistoryState *>(state)->defaultState();
            m_stateWidget->defaultStateComboBox->setCurrentText(defaultState ? defaultState->label() : QLatin1String(""));
        }

        m_stateWidget->onEntryEditor->setPlainText(state->onEntry());
        m_stateWidget->onExitEditor->setPlainText(state->onExit());

        auto *historyState = current<HistoryState>();
        m_stateWidget->historyTypeLabel->setVisible(historyState);
        m_stateWidget->historyTypeEdit->setVisible(historyState);
        if (historyState)
            m_stateWidget->historyTypeEdit->setCurrentIndex(historyState->historyType());

        q->setCurrentIndex(m_stateWidgetIndex); // State page

    } else if (auto *transition = current<Transition>()) {
        m_transitionWidget->labelLineEdit->setText(transition->label());

        m_transitionWidget->sourceStateComboBox->clear();
        State *sourceState = transition->sourceState();
        Q_ASSERT(sourceState);
        if (sourceState) {
            m_transitionWidget->sourceStateComboBox->addItems(allStates(sourceState->machine()));
            m_transitionWidget->sourceStateComboBox->setCurrentText(sourceState->label());
        } else {
            m_transitionWidget->sourceStateComboBox->setCurrentText(QString());
        }
        m_transitionWidget->targetStateComboBox->clear();
        State *targetState = transition->targetState();
        if (sourceState) {
            m_transitionWidget->targetStateComboBox->addItems(allStates(sourceState->machine()));
        } else {
            m_transitionWidget->targetStateComboBox->setCurrentText(QString());
        }
        if (targetState)
            m_transitionWidget->targetStateComboBox->setCurrentText(targetState->label());
        else
            m_transitionWidget->targetStateComboBox->setCurrentText(QString());

        m_transitionWidget->guardEditor->setPlainText(transition->guard());
        q->setCurrentIndex(m_transitionWidgetIndex); // Transition page

        auto *signalTransition = current<SignalTransition>();
        m_transitionWidget->signalLabel->setVisible(signalTransition);
        m_transitionWidget->signalEdit->setVisible(signalTransition);
        if (signalTransition)
            m_transitionWidget->signalEdit->setText(signalTransition->signal());

        auto *timeoutTransition = current<TimeoutTransition>();
        m_transitionWidget->timeoutLabel->setVisible(timeoutTransition);
        m_transitionWidget->timeoutEdit->setVisible(timeoutTransition);
        if (timeoutTransition)
            m_transitionWidget->timeoutEdit->setValue(timeoutTransition->timeout());

    } else {
        q->setCurrentIndex(m_noWidgetIndex);
    }
}

void PropertyEditor::Private::updateSimpleProperty() const
{
    QObject *object = q->sender();
    if (!object || !m_currentElement || !object->metaObject()->userProperty().isValid())
        return;

    const QString propertyName = m_widgetToPropertyMap.value(object);
    Q_ASSERT(!propertyName.isEmpty());

    const QVariant currentValue = m_currentElement->property(propertyName.toUtf8().constData());

    QVariant newValue;
    auto *comboBox = qobject_cast<QComboBox *>(object);
    if (comboBox && currentValue.typeId() != QMetaType::QString) { // the user property on QComboBox is currentString, not always what we want
        newValue = comboBox->currentIndex();
    } else {
        newValue = object->metaObject()->userProperty().read(object);
    }
    if (currentValue == newValue)
        return;

    Q_ASSERT(m_commandController);
    auto *command = new ModifyPropertyCommand(m_currentElement, propertyName.toUtf8().constData(), newValue);
    m_commandController->undoStack()->push(command);
}

void PropertyEditor::Private::setInitalState(const QString &label) const
{
    auto *state = current<State>();
    Q_ASSERT(state);
    if (state) {
        State *currentInitialState = ElementUtil::findInitialState(state);
        State *initialState = ElementUtil::findState(state, label);
        if (currentInitialState != initialState) {
            auto *command = new ModifyInitialStateCommand(state, initialState);
            m_commandController->undoStack()->push(command);
        }
    }
}

void PropertyEditor::Private::setDefaultState(const QString &label) const
{
    auto *state = current<HistoryState>();
    Q_ASSERT(state);
    if (state) {
        State *defaultState = ElementUtil::findState(state->machine(), label);
        if (state->defaultState() != defaultState) {
            auto *command = new ModifyDefaultStateCommand(state, defaultState);
            m_commandController->undoStack()->push(command);
        }
    }
}

void PropertyEditor::Private::setSourceState(const QString &label) const
{
    auto *transition = current<Transition>();
    if (transition) {
        State *sourceState = ElementUtil::findState(transition->sourceState()->machine(), label);
        if (transition->sourceState() != sourceState) {
            auto *command = new ModifyTransitionCommand(transition, m_stateModel);
            command->setSourceState(sourceState);
            m_commandController->undoStack()->push(command);
        }
    }
}

void PropertyEditor::Private::setTargetState(const QString &label) const
{
    auto *transition = current<Transition>();
    if (transition) {
        State *targetState = ElementUtil::findState(transition->sourceState()->machine(), label);
        if (transition->targetState() != targetState) {
            auto *command = new ModifyTransitionCommand(transition, m_stateModel);
            command->setTargetState(targetState);
            m_commandController->undoStack()->push(command);
        }
    }
}

void PropertyEditor::Private::childModeChanged() const
{
    const bool parallelMode = m_stateWidget->childModeEdit->currentIndex() == State::ParallelStates;

    m_stateWidget->initialStateLabel->setEnabled(!parallelMode);
    m_stateWidget->initialStateComboBox->setEnabled(!parallelMode);
}

#include "moc_propertyeditor.cpp"
