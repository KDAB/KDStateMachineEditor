/*
  layoutitem.h

  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
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


#ifndef KDSME_LAYOUT_LAYOUTITEM_H
#define KDSME_LAYOUT_LAYOUTITEM_H

#include "kdsme_core_export.h"

#include <QObject>
#include <QPainterPath>
#include <QPointF>

class QDebug;

namespace KDSME {

class Element;
class Transition;
class TransitionLayoutItem;
class View;

class KDSME_CORE_EXPORT LayoutItem : public QObject
{
    Q_OBJECT
    /// The position of the element from the top-left corner
    Q_PROPERTY(QPointF pos READ pos WRITE setPos NOTIFY posChanged FINAL)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged FINAL)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged FINAL)
    /// Whether this item is visible in the scene
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged FINAL)
    /// Whether this item is marked as selected
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectedChanged FINAL)
    /// The element this item is representing
    Q_PROPERTY(KDSME::Element* element READ element WRITE setElement NOTIFY elementChanged FINAL);
    /// The element type of this object
    Q_PROPERTY(Type type READ type CONSTANT)

public:
    enum Type {
        LayoutType,
        StateType,
        TransitionType
    };
    Q_ENUMS(Type)

    explicit LayoutItem(QObject* parent = 0);
    virtual ~LayoutItem();

    virtual Type type() const;

    QPointF pos() const;
    void setPos(const QPointF& pos);

    qreal height() const;
    void setHeight(qreal height);

    qreal width() const;
    void setWidth(qreal width);

    QPointF absolutePos() const;

    bool isVisible() const;
    void setVisible(bool visible);

    bool isSelected() const;
    void setSelected(bool selected);

    virtual QRectF boundingRect() const;

    View* view() const;

    Element* element() const;
    void setElement(Element* element);

    bool isSiblingOf(LayoutItem* sourceState) const;
    QList<LayoutItem*> childItems() const;
    LayoutItem* parentItem() const;

    virtual QString toDisplayString() const;

Q_SIGNALS:
    void posChanged(const QPointF& pos);
    void heightChanged(qreal height);
    void widthChanged(qreal width);
    void visibleChanged(bool visible);
    void selectedChanged(bool selected);
    void elementChanged(Element* element);

protected:
    void prepareGeometryChange();

    virtual void elementChanged(Element* oldElement, Element* newElement);

private:
    friend class KDSME::View;

    void setView(View* view);

    QPointF m_pos;
    qreal m_height, m_width;
    bool m_visible;
    bool m_selected;

    View* m_view;
    Element* m_element;
};

QDebug operator<<(QDebug dbg, const LayoutItem& item);

class KDSME_CORE_EXPORT StateLayoutItem : public LayoutItem
{
    Q_OBJECT
    Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded NOTIFY expandedChanged FINAL)

public:
    explicit StateLayoutItem(StateLayoutItem* parent = 0);

    virtual Type type() const Q_DECL_OVERRIDE;

    bool isExpanded() const;
    void setExpanded(bool expanded);

    StateLayoutItem* parentState() const;
    QList<StateLayoutItem*> childStates() const;
    QList<TransitionLayoutItem*> transitions() const;

Q_SIGNALS:
    void expandedChanged(bool expanded);

private:
    bool m_expanded;
};

QDebug operator<<(QDebug dbg, const StateLayoutItem& item);

class KDSME_CORE_EXPORT TransitionLayoutItem : public LayoutItem
{
    Q_OBJECT
    /// The exact shape of this transition
    Q_PROPERTY(QPainterPath shape READ shape WRITE setShape NOTIFY shapeChanged FINAL)
    Q_PROPERTY(QRectF labelBoundingRect READ labelBoundingRect WRITE setLabelBoundingRect NOTIFY labelBoundingRectChanged FINAL)
    Q_PROPERTY(KDSME::StateLayoutItem* targetState READ targetState WRITE setTargetState NOTIFY targetStateChanged FINAL)

public:
    explicit TransitionLayoutItem(StateLayoutItem* parent = 0);

    virtual Type type() const Q_DECL_OVERRIDE;

    QPainterPath shape() const;
    void setShape(const QPainterPath& path);

    QRectF labelBoundingRect() const;
    void setLabelBoundingRect(const QRectF& rect);

    void moveToState(StateLayoutItem* state);

    StateLayoutItem* sourceState() const;

    StateLayoutItem* targetState() const;
    void setTargetState(StateLayoutItem* state);

    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;

protected:
    virtual void elementChanged(Element* oldElement, Element* newElement);

Q_SIGNALS:
    void shapeChanged(const QPainterPath& path);
    void labelBoundingRectChanged(const QRectF& rect);
    void targetStateChanged(StateLayoutItem* state);

private Q_SLOTS:
    void updateItem();

private:
    KDSME::Transition* transition() const;

    QPainterPath m_shape;
    QRectF m_labelBoundingRect;
    StateLayoutItem* m_targetState;
};

QDebug operator<<(QDebug dbg, const TransitionLayoutItem& item);

}

Q_DECLARE_METATYPE(KDSME::LayoutItem*);

#endif
