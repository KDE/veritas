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

#include "selectionstore.h"
#include "../test.h"
#include "test_p.h"
#include "utils.h"
#include <KDebug>

using Veritas::SelectionStore;
using Veritas::Test;

namespace {

class SaveTest
{
public:
    SaveTest(SelectionStore* store) : m_store(store) {}
    void operator()(Test* t) {
        Q_ASSERT(m_store);
        m_store->saveState(t);
    }
    SelectionStore* m_store;
};

class RestoreTest
{
public:
    RestoreTest(SelectionStore* store) : m_store(store) {}
    void operator()(Test* t) {
        Q_ASSERT(m_store);
        if (m_store->wasDeselectedLeaf(t)) {
            t->internal()->setCheckState(Qt::Unchecked);
        }
    }
    SelectionStore* m_store;
};

}

void SelectionStore::saveState(Test* test)
{
    Q_ASSERT(test);
    if (test->childCount() > 0)
    {
      // We only have to store the state for the leafs
      return;
    }
    if (test->internal()->checkState() == Qt::Unchecked) {
        m_deselectedLeaves << serialize(test);
    }
}

QString SelectionStore::serialize(Test* test) const
{
    Q_ASSERT(test);
    QString serialized = test->name();
    Test* parent = test->parent();
    while (parent) {
        serialized = parent->name() + '/' + serialized;
        parent = parent->parent();
    }
    return serialized;
}

bool SelectionStore::wasDeselectedLeaf(Test* test)
{
    Q_ASSERT(test);
    return m_deselectedLeaves.contains(serialize(test));
}

void SelectionStore::saveTree(Test* root)
{
    SaveTest st(this);
    traverseTree(root, st);
}

void SelectionStore::restoreTree(Test* root)
{
    RestoreTest rt(this);
    for(int i=0; i<root->childCount(); i++) {
        traverseTree(root->child(i), rt);
    }
}

