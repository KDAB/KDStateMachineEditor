/*
  This file is part of the KDAB State Machine Editor Library.

  SPDX-FileCopyrightText: 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: LGPL-2.1-only OR LicenseRef-KDAB-KDStateMachineEditor

  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "objecttreemodel.h"

#include <QDebug>
#include <QSignalSpy>
#include <QTest>

using namespace KDSME;

namespace {

/**
 * Return a sample QObject tree
 *
 * <pre>
 * Hierarchy:
 * - root
 *   - o1
 *   - o2
 * </pre>
 */
QObject* createQObjectTreeSample()
{
    QObject* o1 = new QObject;
    o1->setObjectName("root");
    QObject* o11 = new QObject(o1);
    o11->setObjectName("o1");
    QObject* o12 = new QObject(o1);
    o12->setObjectName("o2");
    return o1;
}

}

class ModelsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testObjectTreeModel();
    void testObjectTreeModel_AppendOperation_SingleObject();
    void testObjectTreeModel_AppendOperation_MultipleObjects();
    void testObjectTreeModel_RemoveOperation_SingleObject();
    void testObjectTreeModel_ResetOperation_SingleObject();
    void testObjectTreeModel_ReparentOperation_SingleObject();
    void testObjectTreeModel_ReparentOperation_SingleObject_Invalid();
};

void ModelsTest::testObjectTreeModel()
{
    ObjectTreeModel model;
    QCOMPARE(model.rowCount(), 0);

    QScopedPointer<QObject> root(createQObjectTreeSample());
    model.appendRootObject(root.data());
    QCOMPARE(model.rowCount(), 1);
    QModelIndex rootIndex = model.index(0, 0);
    QCOMPARE(model.rowCount(rootIndex), 2);
    QCOMPARE(rootIndex.data(ObjectTreeModel::ObjectRole).value<QObject*>(),  root.data());
    QModelIndex index = model.index(0, 0, rootIndex);
    QCOMPARE(model.rowCount(index), 0);
    QCOMPARE(index.data(ObjectTreeModel::ObjectRole).value<QObject*>(),  root->children()[0]);
    index = model.index(1, 0, rootIndex);
    QCOMPARE(model.rowCount(index), 0);
    QCOMPARE(index.data(ObjectTreeModel::ObjectRole).value<QObject*>(),  root->children()[1]);
}


void ModelsTest::testObjectTreeModel_AppendOperation_SingleObject()
{
    QScopedPointer<QObject> root(createQObjectTreeSample());
    ObjectTreeModel model;
    model.appendRootObject(root.data());

    QSignalSpy spy(&model, SIGNAL(rowsInserted(QModelIndex,int,int)));
    QObject* appendedObject;
    {
        ObjectTreeModel::AppendOperation insert(&model, root.data());
        appendedObject = new QObject(root.data());
        appendedObject->setObjectName("appendedObject");
    }

    QCOMPARE(spy.count(), 1);
    auto args = spy.takeFirst();
    const QModelIndex parentIndex = args[0].value<QModelIndex>();
    const int first = args[1].toInt();
    const int last = args[2].toInt();
    QCOMPARE(parentIndex.data(ObjectTreeModel::ObjectRole).value<QObject*>(), root.data());
    QCOMPARE(first, 2);
    QCOMPARE(last, 2);

    QCOMPARE(model.index(first, 0, parentIndex).data(ObjectTreeModel::ObjectRole).value<QObject*>(), appendedObject);
}

void ModelsTest::testObjectTreeModel_AppendOperation_MultipleObjects()
{
    QScopedPointer<QObject> root(createQObjectTreeSample());
    ObjectTreeModel model;
    model.appendRootObject(root.data());

    QSignalSpy spy(&model, SIGNAL(rowsInserted(QModelIndex,int,int)));
    QList<QObject*> appendedObjects;
    {
        ObjectTreeModel::AppendOperation insert(&model, root.data(), 3);
        appendedObjects << new QObject(root.data());
        appendedObjects << new QObject(root.data());
        appendedObjects << new QObject(root.data());
    }

    QCOMPARE(spy.count(), 1);
    auto args = spy.takeFirst();
    const QModelIndex parentIndex = args[0].value<QModelIndex>();
    const int first = args[1].toInt();
    const int last = args[2].toInt();
    QCOMPARE(parentIndex.data(ObjectTreeModel::ObjectRole).value<QObject*>(), root.data());
    QCOMPARE(first, 2);
    QCOMPARE(last, 4);

    QCOMPARE(model.index(first, 0, parentIndex).data(ObjectTreeModel::ObjectRole).value<QObject*>(), appendedObjects[0]);
    QCOMPARE(model.index(first+1, 0, parentIndex).data(ObjectTreeModel::ObjectRole).value<QObject*>(), appendedObjects[1]);
    QCOMPARE(model.index(first+2, 0, parentIndex).data(ObjectTreeModel::ObjectRole).value<QObject*>(), appendedObjects[2]);
}

void ModelsTest::testObjectTreeModel_RemoveOperation_SingleObject()
{
    QScopedPointer<QObject> root(createQObjectTreeSample());
    ObjectTreeModel model;
    model.appendRootObject(root.data());

    QSignalSpy spy(&model, SIGNAL(rowsRemoved(QModelIndex,int,int)));
    {
        QObject* object = root.data()->children()[0];
        ObjectTreeModel::RemoveOperation remove(&model, object);
        delete object;
    }

    QCOMPARE(spy.count(), 1);
    auto args = spy.takeFirst();
    const QModelIndex parentIndex = args[0].value<QModelIndex>();
    const int first = args[1].toInt();
    const int last = args[2].toInt();
    QCOMPARE(parentIndex.data(ObjectTreeModel::ObjectRole).value<QObject*>(), root.data());
    QCOMPARE(first, 0);
    QCOMPARE(last, 0);
}

void ModelsTest::testObjectTreeModel_ResetOperation_SingleObject()
{
    QScopedPointer<QObject> root(createQObjectTreeSample());
    ObjectTreeModel model;
    model.appendRootObject(root.data());

    QSignalSpy spy1(&model, SIGNAL(modelAboutToBeReset()));
    QSignalSpy spy2(&model, SIGNAL(modelAboutToBeReset()));
    {
        QObject* object = root.data()->children()[0];
        ObjectTreeModel::ResetOperation reset(&model);
        QCOMPARE(spy1.count(), 1);
        delete object;
    }
    QCOMPARE(spy2.count(), 1);
}

void ModelsTest::testObjectTreeModel_ReparentOperation_SingleObject()
{
    QScopedPointer<QObject> root(createQObjectTreeSample());
    ObjectTreeModel model;
    model.appendRootObject(root.data());

    QObject* o1 = root.data()->children()[0];
    QVERIFY(o1);
    QObject* o2 = root.data()->children()[1];
    QVERIFY(o2);

    // Want to set parent of o2 to o1
    QSignalSpy spy1(&model, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)));
    QSignalSpy spy2(&model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)));
    {
        ObjectTreeModel::ReparentOperation reparent(&model, o2, o1);
        QCOMPARE(spy1.count(), 1);
        o2->setParent(o1);
    }
    QCOMPARE(spy2.count(), 1);

    {
        auto args = spy1.takeFirst();
        const QModelIndex sourceParent = args[0].value<QModelIndex>();
        const int sourceStart = args[1].toInt();
        const int sourceEnd = args[2].toInt();
        const QModelIndex destinationParent = args[3].value<QModelIndex>();
        const int destinationRow = args[4].toInt();
        QCOMPARE(sourceParent, model.indexForObject(root.data()));
        QCOMPARE(sourceStart, 1);
        QCOMPARE(sourceEnd, 1);
        QCOMPARE(destinationParent, model.indexForObject(o1));
        QCOMPARE(destinationRow, 0);
    }

    const QModelIndex rootIndex = model.index(0, 0);
    const QModelIndex o1Index = model.index(0, 0, rootIndex);
    const QModelIndex o2Index = model.index(0, 0, o1Index);
    QCOMPARE(o1Index.data(ObjectTreeModel::ObjectRole).value<QObject*>(), o1);
    QCOMPARE(o2Index.data(ObjectTreeModel::ObjectRole).value<QObject*>(), o2);
}

void ModelsTest::testObjectTreeModel_ReparentOperation_SingleObject_Invalid()
{
    QScopedPointer<QObject> root(createQObjectTreeSample());
    ObjectTreeModel model;
    model.appendRootObject(root.data());

    QObject* o1 = root.data()->children()[0];
    QVERIFY(o1);
    QObject* o2 = root.data()->children()[1];
    QVERIFY(o2);

    {
        // Want to set parent of o1 to root (root is already parent of o1!)
        QSignalSpy spy1(&model, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)));
        {
            ObjectTreeModel::ReparentOperation reparent(&model, o1, root.data());
            QCOMPARE(spy1.count(), 0);
            o2->setParent(o1);
        }
    }

    {
        // Want to set parent of o1 to o1 (circular link!)
        QSignalSpy spy1(&model, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int)));
        {
            ObjectTreeModel::ReparentOperation reparent(&model, o1, o1);
            QCOMPARE(spy1.count(), 0);
            o2->setParent(o1);
        }
    }
}

QTEST_MAIN(ModelsTest)

#include "test_models.moc"
