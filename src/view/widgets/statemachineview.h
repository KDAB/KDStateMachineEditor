/*
  statemachineview.h

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

#ifndef KDSME_WIDGETS_STATEMACHINEVIEW_H
#define KDSME_WIDGETS_STATEMACHINEVIEW_H

#include "kdsme_view_export.h"

#include <QQuickWidget>

namespace KDSME {

class Command;
class EditController;
class CommandController;
class ConfigurationController;
class LayoutItem;
class View;
class StateMachine;
class Element;

class KDSME_VIEW_EXPORT StateMachineView : public QQuickWidget
{
    Q_OBJECT
    Q_PROPERTY(KDSME::View* view READ view WRITE setView NOTIFY viewChanged)
    Q_PROPERTY(KDSME::CommandController* commandController READ commandController CONSTANT)
    Q_PROPERTY(KDSME::ConfigurationController* configurationController READ configurationController CONSTANT)
    Q_PROPERTY(KDSME::EditController* editController READ editController CONSTANT)

public:
    explicit StateMachineView(QWidget* parent = 0);
    virtual ~StateMachineView();

    View* view() const;
    void setView(View* m_view);

    CommandController* commandController() const;
    ConfigurationController* configurationController() const;
    EditController* editController() const;

    Q_INVOKABLE void sendCommand(KDSME::Command* cmd);

    Q_INVOKABLE qreal zoom() const;
    Q_INVOKABLE void setZoom(qreal value);
    Q_INVOKABLE void fitInView(const QRectF& rect = QRectF());

    Q_INVOKABLE void changeStateMachine(KDSME::StateMachine *stateMachine);
    Q_INVOKABLE void deleteElement(KDSME::Element *element);

protected:
    QQuickItem* viewPortObject() const;
    QQuickItem* sceneObject() const;

Q_SIGNALS:
    void viewChanged(KDSME::View* view);

private:
    struct Private;
    QScopedPointer<Private> d;

    Q_PRIVATE_SLOT(d, void onRootLayoutItemChanged(KDSME::LayoutItem*))
    Q_PRIVATE_SLOT(d, void onStateMachineChanged(KDSME::StateMachine*))
};

}

#endif // STARTCHARTVIEW_H
