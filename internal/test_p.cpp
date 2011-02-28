/*
* KDevelop xUnit integration
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

#include "test_p.h"
#include <KDebug>

using Veritas::Test;

const int Test::Internal::columnCount = 4;

Test::Internal::Internal(Veritas::Test* self)
  : self(self)
{
    m_isRunning = false;
}

void Test::Internal::setIndex(const QModelIndex& index)
{
    index_ = index;
}
QModelIndex Test::Internal::index() const
{
    return index_;
}

QVariant Test::Internal::data(int column) const
{
    if (column == 0) {
        return name;
    } else {
        return itemData.value(column);
    }
}

void Test::Internal::setData(int column, const QVariant& value)
{
    if (column == 0) {
        name = value.toString();
    } else if (column > 0 && column < Internal::columnCount ) {
        itemData.replace(column, value.toString());
    }
}

void Test::Internal::clear()
{
    // Initialize columns except column 0 which contains the item name.
    for (int i = 1; i < columnCount; i++) {
        setData(i, "");
    }
    if (result) delete result;
    result = new TestResult;
    m_isRunning = false;
}

void Test::Internal::setCheckState(Qt::CheckState state)
{
    if (checkState_ != state) {
        checkState_ = state;
        if (state != Qt::PartiallyChecked) {
            foreach (Test* child, children) {
                child->internal()->setCheckState(state);
            }
        }
        if (self && self->parent()) {
            self->parent()->internal()->updateCheckState();
        }
    }
}

Qt::CheckState Test::Internal::checkState() const
{
    return checkState_;
}

void Test::Internal::updateCheckState()
{
    Qt::CheckState newState = checkState_;
    if (checkState_ == Qt::PartiallyChecked && children.isEmpty()) {
        newState = Qt::Checked;
    }
    else if (!children.isEmpty()) {
        bool checkedChildren = false;
        bool unCheckedChildren = false;
        foreach (Test* child, children) {
            child->internal()->updateCheckState();
            checkedChildren |= child->internal()->checkState() != Qt::Unchecked;
            unCheckedChildren |= child->internal()->checkState() != Qt::Checked;
            if (checkedChildren && unCheckedChildren) {
                newState = Qt::PartiallyChecked;
                break;
            }
        }
        if (checkedChildren && !unCheckedChildren) {
            newState = Qt::Checked;
        }
        else if (unCheckedChildren && !checkedChildren) {
            newState = Qt::Unchecked;
        }
    }
    setCheckState(newState);
}

bool Test::Internal::isRunning() const
{
    return m_isRunning;
}

void Test::Internal::setIsRunning(bool value)
{
    m_isRunning = value;
}

