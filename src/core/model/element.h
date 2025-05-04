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

#ifndef KDSME_MODEL_ELEMENT_H
#define KDSME_MODEL_ELEMENT_H

#include "kdsme_core_export.h"

#include <QObject>
#include <QMetaType>
#include <QPointF>

QT_BEGIN_NAMESPACE
class QPainterPath;
QT_END_NAMESPACE

namespace KDSME {

class State;
class StateMachine;

class KDSME_CORE_EXPORT Element : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KDSME::Element *parent READ parentElement WRITE setParentElement NOTIFY parentChanged FINAL)
    Q_PROPERTY(Type type READ type CONSTANT FINAL)
    Q_PROPERTY(Flags flags READ flags WRITE setFlags NOTIFY flagsChanged FINAL)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged FINAL)
    /// The position of the element from the top-left corner
    Q_PROPERTY(QPointF pos READ pos WRITE setPos NOTIFY posChanged FINAL)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged FINAL)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged FINAL)
    /// Whether this item is visible in the scene
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged FINAL)
    /// Whether this item is marked as selected
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged FINAL)

public:
    enum Type
    {
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
    Q_ENUM(Type)

    enum Flag
    {
        NoFlags = 0x0,
        ElementIsDragEnabled = 0x1,
        ElementIsSelectable = 0x2,
        ElementIsEditable = 0x4,
    };
    Q_ENUM(Flag)
    Q_DECLARE_FLAGS(Flags, Flag)

    explicit Element(QObject *parent = nullptr);
    ~Element();

    virtual Type type() const;

    Flags flags() const;
    void setFlags(Flags flags);

    /**
     * Label of this state (required to be unique)
     */
    QString label() const;
    void setLabel(const QString &label);

    /**
     * Internal ID (e.g. memory address)
     *
     * Internal id you can use to keep a mapping between elements and your data structures
     */
    quintptr internalId() const;
    void setInternalId(quintptr id);

    void setInternalPointer(void *ptr);
    void *internalPointer() const;

    QPointF pos() const;
    void setPos(const QPointF &pos);

    qreal height() const;
    void setHeight(qreal height);

    qreal width() const;
    void setWidth(qreal width);

    QPointF absolutePos() const;

    bool isVisible() const;
    void setVisible(bool visible);

    bool isSelected() const;
    void setSelected(bool selected);

    QSizeF preferredSize() const;
    virtual QRectF boundingRect() const;

    Element *parentElement() const;
    void setParentElement(Element *parent);
    void setParent(QObject *object); // hide parent function
    QList<Element *> childElements() const;

    virtual QString toDisplayString() const;

    Q_INVOKABLE static const char *typeToString(KDSME::Element::Type type);
    Q_INVOKABLE static KDSME::Element::Type stringToType(const char *type);

Q_SIGNALS:
    void parentChanged(KDSME::Element *parent);
    void flagsChanged(KDSME::Element::Flags flags);
    void labelChanged(const QString &label);
    void posChanged(const QPointF &pos);
    void heightChanged(qreal height);
    void widthChanged(qreal width);
    void visibleChanged(bool visible);
    void selectedChanged(bool selected);

private:
    struct Private;
    QScopedPointer<Private> d;
};

}

Q_DECLARE_METATYPE(KDSME::Element::Type)
Q_DECLARE_METATYPE(KDSME::Element *)

#endif // TREESTRUCTURE_H
