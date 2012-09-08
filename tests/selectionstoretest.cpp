/* KDevelop xUnit plugin
 *
 * Copyright 2008 Manuel Breugelmans <mbr.nxi@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */


#include "selectionstoretest.h"
#include <QtTest/QTest>

#include "../test.h"
#include "../internal/test_p.h"
#include "../internal/selectionstore.h"
#include "kasserts.h"

using Veritas::SelectionStoreTest;

void SelectionStoreTest::init()
{
    m_store = new SelectionStore;
    m_root = new Test("root", 0);
}

void SelectionStoreTest::cleanup()
{
    delete m_store;
    if (m_root) delete m_root;
}

// TODO refactor this mess

void SelectionStoreTest::rootOnly()
{
    m_root->internal()->setCheckState(Qt::Checked);
    m_store->saveState(m_root);
    KVERIFY(!m_store->wasDeselectedLeaf(m_root));

    m_root->internal()->setCheckState(Qt::Unchecked);
    m_store->saveState(m_root);
    KVERIFY(m_store->wasDeselectedLeaf(m_root));
}

void SelectionStoreTest::newObject()
{
    m_root->internal()->setCheckState(Qt::Unchecked);
    m_store->saveState(m_root);

    Test* root2 = new Test("root", 0);
    KVERIFY(m_store->wasDeselectedLeaf(root2));

    delete root2;
}

void SelectionStoreTest::saveMultiple()
{
    Test* test1 = new Test("test1", 0);
    Test* test2 = new Test("test2", 0);
    Test* test3 = new Test("test3", 0);

    test1->internal()->setCheckState(Qt::Unchecked);
    test2->internal()->setCheckState(Qt::Checked);
    test3->internal()->setCheckState(Qt::Unchecked);

    m_store->saveState(test1);
    m_store->saveState(test2);
    m_store->saveState(test3);

    KVERIFY(m_store->wasDeselectedLeaf(test1));
    KVERIFY(!m_store->wasDeselectedLeaf(test2));
    KVERIFY(m_store->wasDeselectedLeaf(test3));

    delete test1;
    delete test2;
    delete test3;
}

void SelectionStoreTest::testTree()
{
    Test* child1 = new Test("test2", m_root);
    Test* child2 = new Test("test3", m_root);

    child1->internal()->setCheckState(Qt::Checked);
    child2->internal()->setCheckState(Qt::Unchecked);

    m_store->saveState(child1);
    m_store->saveState(child2);

    KVERIFY(!m_store->wasDeselectedLeaf(child1));
    KVERIFY(m_store->wasDeselectedLeaf(child2));
}

void SelectionStoreTest::testTreeWithIdenticalNames()
{
    Test* child1 = new Test("child", m_root);
    Test* root2 = new Test("root2", 0);
    Test* child2 = new Test("child", root2);

    child1->internal()->setCheckState(Qt::Checked);
    child2->internal()->setCheckState(Qt::Unchecked);

    m_store->saveState(child1);
    m_store->saveState(child2);

    KVERIFY(!m_store->wasDeselectedLeaf(child1));
    KVERIFY(m_store->wasDeselectedLeaf(child2));

    delete root2;
}

void SelectionStoreTest::saveRecursive()
{
    /* child1      checked
     * child2      not checked
     *   - child21 not checked
     */

    Test* child1 = new Test("test2", m_root);
    Test* child2 = new Test("test3", m_root);
    Test* child21 = new Test("test21", child2);
    m_root->addChild(child1);
    m_root->addChild(child2);
    child2->addChild(child21);

    m_root->internal()->setCheckState(Qt::Checked);
    child1->internal()->setCheckState(Qt::Checked);
    child2->internal()->setCheckState(Qt::Unchecked);
    child21->internal()->setCheckState(Qt::Unchecked);

    m_store->saveTree(m_root);

    delete m_root;
    m_root = new Test("root", 0);
    child1 = new Test("test2", m_root);
    child2 = new Test("test3", m_root);
    child21 = new Test("test21", child2);

    KVERIFY(!m_store->wasDeselectedLeaf(m_root));
    KVERIFY(!m_store->wasDeselectedLeaf(child1));
    KVERIFY(!m_store->wasDeselectedLeaf(child2));
    KVERIFY(m_store->wasDeselectedLeaf(child21));
}


void SelectionStoreTest::restoreRecursive()
{
    Test* child1 = new Test("test2", m_root);
    m_root->addChild(child1);
    Test* child2 = new Test("test3", m_root);
    m_root->addChild(child2);
    Test* child21 = new Test("test21", child2);
    child2->addChild(child21);

    m_root->internal()->setCheckState(Qt::Checked);
    child1->internal()->setCheckState(Qt::Checked);
    child2->internal()->setCheckState(Qt::Unchecked);
    child21->internal()->setCheckState(Qt::Unchecked);

    m_store->saveTree(m_root);

    // reset the tree
    delete m_root;
    m_root = new Test("root", 0);
    child1 = new Test("test2", m_root);
    m_root->addChild(child1);
    child2 = new Test("test3", m_root);
    m_root->addChild(child2);
    child21 = new Test("test21", child2);
    child2->addChild(child21);

    // insert some more
    Test* child3 = new Test("child3", m_root);
    m_root->addChild(child3);
    Test* child22 = new Test("child22", child2);
    child2->addChild(child22);

    m_store->restoreTree(m_root);

    KVERIFY(m_root->internal()->checkState()==Qt::PartiallyChecked);
    KVERIFY(child1->internal()->checkState()==Qt::Checked);
    KVERIFY(child2->internal()->checkState()==Qt::PartiallyChecked);
    KVERIFY(child21->internal()->checkState()==Qt::Unchecked);
    KVERIFY(child22->internal()->checkState()==Qt::Checked); // newly added test, so checked
    KVERIFY(child3->internal()->checkState()==Qt::Checked);
}

void SelectionStoreTest::ignoreRoot()
{
    // The invisible m_root of the test-tree should not be saved/restored.
    m_root->internal()->setCheckState(Qt::Checked);
    m_store->saveTree(m_root);

    Test* root2 = new Test("root", 0);
    m_store->restoreTree(root2);
    KVERIFY(root2->internal()->checkState()==Qt::Checked);

    m_root->internal()->setCheckState(Qt::Unchecked);
    m_store->saveTree(m_root);

    delete root2;
    root2 = new Test("root", 0);
    m_store->restoreTree(root2);
    KVERIFY(root2->internal()->checkState()==Qt::Checked);

    delete root2;
}

void SelectionStoreTest::selectedChildDeselectedParent()
{
    /* parent       deselected
     *   - child    selected
     *
     * First serialize the selection state of this test-tree
     * then restore it. Both parent and child should be selected, as the parent
     * has only one selected child, and the checked state of the child is
     * propagated to the parent.
     */
    Test* parent = new Test("parent", m_root);
    m_root->addChild(parent);
    Test* child = new Test("child", parent);
    parent->addChild(child);

    parent->internal()->setCheckState(Qt::Unchecked);
    child->internal()->setCheckState(Qt::Checked);

    m_store->saveTree(m_root);

    // reset the tree
    delete m_root;
    m_root = new Test("root", 0);
    parent = new Test("parent", m_root);
    m_root->addChild(parent);
    child = new Test("child", parent);
    parent->addChild(child);

    m_store->restoreTree(m_root);

    KVERIFY(parent->internal()->checkState()==Qt::Checked);
    KVERIFY(child->internal()->checkState()==Qt::Checked);
}

QTEST_MAIN( SelectionStoreTest )
#include "selectionstoretest.moc"
