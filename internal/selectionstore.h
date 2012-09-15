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

#ifndef VERITAS_SELECTIONSTORE_H
#define VERITAS_SELECTIONSTORE_H

#include <QSet>
#include "../veritasexport.h"

namespace Veritas
{
class Test;

/*! Store select state of a test tree. Used to retrieve this state after reload
    @unittest Veritas::SelectionStoreTest */
class VERITAS_EXPORT SelectionStore
{
public:
    void saveState(Test* test);
    void saveTree(Test* root);

    bool wasDeselectedLeaf(Test* test);
    void restoreTree(Test* root);

private:
    QString serialize(Test*) const;

private:
    QSet<QString> m_deselectedLeaves; // serialized name of deselected leaf tests
};

}

#endif // VERITAS_SELECTIONSTORE_H
