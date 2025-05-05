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
QObject *createQObjectTreeSample()
{
    auto *o1 = new QObject;
    o1->setObjectName(QStringLiteral("root"));
    auto *o11 = new QObject(o1);
    o11->setObjectName(QStringLiteral("o1"));
    auto *o12 = new QObject(o1);
    o12->setObjectName(QStringLiteral("o2"));
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

    const QScopedPointer<QObject> root(createQObjectTreeSample());
    model.appendRootObject(root.data());
    QCOMPARE(model.rowCount(), 1);
    const QModelIndex rootIndex = model.index(0, 0);
    QCOMPARE(model.rowCount(rootIndex), 2);
    QCOMPARE(rootIndex.data(ObjectTreeModel::ObjectRole).value<QObject *>(), root.data());
    QModelIndex index = model.index(0, 0, rootIndex);
    QCOMPARE(model.rowCount(index), 0);
    QCOMPARE(index.data(ObjectTreeModel::ObjectRole).value<QObject *>(), root->children()[0]);
    index = model.index(1, 0, rootIndex);
    QCOMPARE(model.rowCount(index), 0);
    QCOMPARE(index.data(ObjectTreeModel::ObjectRole).value<QObject *>(), root->children()[1]);
}


void ModelsTest::testObjectTreeModel_AppendOperation_SingleObject()
{
    const QScopedPointer<QObject> root(createQObjectTreeSample());
    ObjectTreeModel model;
    model.appendRootObject(root.data());

    // clang-format off
    QSignalSpy spy(&model, SIGNAL(rowsInserted(QModelIndex,int,int))); // clazy:exclude=old-style-connect
    // clang-format on
    QObject *appendedObject;
    {
        const ObjectTreeModel::AppendOperation insert(&model, root.data());
        appendedObject = new QObject(root.data());
        appendedObject->setObjectName(QStringLiteral("appendedObject"));
    }

    QCOMPARE(spy.count(), 1);
    auto args = spy.takeFirst();
    const auto parentIndex = args[0].value<QModelIndex>();
    const int first = args[1].toInt();
    const int last = args[2].toInt();
    QCOMPARE(parentIndex.data(ObjectTreeModel::ObjectRole).value<QObject *>(), root.data());
    QCOMPARE(first, 2);
    QCOMPARE(last, 2);

    QCOMPARE(model.index(first, 0, parentIndex).data(ObjectTreeModel::ObjectRole).value<QObject *>(), appendedObject);
}

void ModelsTest::testObjectTreeModel_AppendOperation_MultipleObjects()
{
    const QScopedPointer<QObject> root(createQObjectTreeSample());
    ObjectTreeModel model;
    model.appendRootObject(root.data());

    // clang-format off
    QSignalSpy spy(&model, SIGNAL(rowsInserted(QModelIndex,int,int))); // clazy:exclude=old-style-connect
    // clang-format on
    QList<QObject *> appendedObjects;
    {
        const ObjectTreeModel::AppendOperation insert(&model, root.data(), 3);
        appendedObjects << new QObject(root.data());
        appendedObjects << new QObject(root.data());
        appendedObjects << new QObject(root.data());
    }

    QCOMPARE(spy.count(), 1);
    auto args = spy.takeFirst();
    const auto parentIndex = args[0].value<QModelIndex>();
    const int first = args[1].toInt();
    const int last = args[2].toInt();
    QCOMPARE(parentIndex.data(ObjectTreeModel::ObjectRole).value<QObject *>(), root.data());
    QCOMPARE(first, 2);
    QCOMPARE(last, 4);

    QCOMPARE(model.index(first, 0, parentIndex).data(ObjectTreeModel::ObjectRole).value<QObject *>(), appendedObjects[0]);
    QCOMPARE(model.index(first + 1, 0, parentIndex).data(ObjectTreeModel::ObjectRole).value<QObject *>(), appendedObjects[1]);
    QCOMPARE(model.index(first + 2, 0, parentIndex).data(ObjectTreeModel::ObjectRole).value<QObject *>(), appendedObjects[2]);
}

void ModelsTest::testObjectTreeModel_RemoveOperation_SingleObject()
{
    const QScopedPointer<QObject> root(createQObjectTreeSample());
    ObjectTreeModel model;
    model.appendRootObject(root.data());

    // clang-format off
    QSignalSpy spy(&model, SIGNAL(rowsRemoved(QModelIndex, int, int))); // clazy:exclude=old-style-connect
    // clang-format on
    {
        QObject *object = root.data()->children()[0];
        const ObjectTreeModel::RemoveOperation remove(&model, object);
        delete object;
    }

    QCOMPARE(spy.count(), 1);
    auto args = spy.takeFirst();
    const auto parentIndex = args[0].value<QModelIndex>();
    const int first = args[1].toInt();
    const int last = args[2].toInt();
    QCOMPARE(parentIndex.data(ObjectTreeModel::ObjectRole).value<QObject *>(), root.data());
    QCOMPARE(first, 0);
    QCOMPARE(last, 0);
}

void ModelsTest::testObjectTreeModel_ResetOperation_SingleObject()
{
    const QScopedPointer<QObject> root(createQObjectTreeSample());
    ObjectTreeModel model;
    model.appendRootObject(root.data());

    const QSignalSpy spy1(&model, SIGNAL(modelAboutToBeReset())); // clazy:exclude=old-style-connect
    const QSignalSpy spy2(&model, SIGNAL(modelAboutToBeReset())); // clazy:exclude=old-style-connect
    {
        QObject *object = root.data()->children()[0];
        const ObjectTreeModel::ResetOperation reset(&model);
        QCOMPARE(spy1.count(), 1);
        delete object;
    }
    QCOMPARE(spy2.count(), 1);
}

void ModelsTest::testObjectTreeModel_ReparentOperation_SingleObject() // NOLINT(readability-function-cognitive-complexity)
{
    const QScopedPointer<QObject> root(createQObjectTreeSample());
    ObjectTreeModel model;
    model.appendRootObject(root.data());

    QObject *o1 = root.data()->children()[0];
    QVERIFY(o1);
    QObject *o2 = root.data()->children()[1];
    QVERIFY(o2);

    // Want to set parent of o2 to o1
    // clang-format off
    QSignalSpy spy1(&model, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int))); // clazy:exclude=old-style-connect
    const QSignalSpy spy2(&model, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int))); // clazy:exclude=old-style-connect
    // clang-format on
    {
        const ObjectTreeModel::ReparentOperation reparent(&model, o2, o1);
        QCOMPARE(spy1.count(), 1);
        o2->setParent(o1);
    }
    QCOMPARE(spy2.count(), 1);

    {
        auto args = spy1.takeFirst();
        const auto sourceParent = args[0].value<QModelIndex>();
        const int sourceStart = args[1].toInt();
        const int sourceEnd = args[2].toInt();
        const auto destinationParent = args[3].value<QModelIndex>();
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
    QCOMPARE(o1Index.data(ObjectTreeModel::ObjectRole).value<QObject *>(), o1);
    QCOMPARE(o2Index.data(ObjectTreeModel::ObjectRole).value<QObject *>(), o2);
}

void ModelsTest::testObjectTreeModel_ReparentOperation_SingleObject_Invalid()
{
    const QScopedPointer<QObject> root(createQObjectTreeSample());
    ObjectTreeModel model;
    model.appendRootObject(root.data());

    QObject *o1 = root.data()->children()[0];
    QVERIFY(o1);
    QObject *o2 = root.data()->children()[1];
    QVERIFY(o2);

    {
        // Want to set parent of o1 to root (root is already parent of o1!)
        // clang-format off
        const QSignalSpy spy1(&model, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int))); // clazy:exclude=old-style-connect
        // clang-format on
        {
            const ObjectTreeModel::ReparentOperation reparent(&model, o1, root.data());
            QCOMPARE(spy1.count(), 0);
            o2->setParent(o1);
        }
    }

    {
        // Want to set parent of o1 to o1 (circular link!
        // clang-format off
        const QSignalSpy spy1(&model, SIGNAL(rowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int))); // clazy:exclude=old-style-connect
        // clang-format on
        {
            const ObjectTreeModel::ReparentOperation reparent(&model, o1, o1);
            QCOMPARE(spy1.count(), 0);
            o2->setParent(o1);
        }
    }
}

QTEST_MAIN(ModelsTest)

#include "test_models.moc"
