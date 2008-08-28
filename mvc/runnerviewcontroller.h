/* KDevelop xUnit plugin
 *
 * Copyright 2006 systest.ch <qxrunner@systest.ch>
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
 
/*!
 * \file  runnerviewcontroller.h
 *
 * \brief Declares class RunnerViewController.
 */

#ifndef VERITAS_RUNNERVIEWCONTROLLER_H
#define VERITAS_RUNNERVIEWCONTROLLER_H

#include "veritas/mvc/viewcontrollercommon.h"
#include "../veritasexport.h"
#include <QObject>

namespace Veritas
{

class RunnerModel;
class RunnerProxyModel;

/*!
 * \brief The RunnerViewController class provides functionality for
 *        the runner view.
 *
 * This helper class is introduced to avoid bloating the main window
 * class with code specific for views representing data from RunnerModel.
 *
 * Most methods of the class facilitate user interaction with the view.
 * Sorting of data by clicking on a column header isn't supported.
 *
 * \sa \ref views and \ref selected_item
 */
class VERITAS_EXPORT RunnerViewController : public ViewControllerCommon
{
    Q_OBJECT

public: // Operations

    /*!
     * Constructs a runner view controller with the given \a parent
     * and \a view.
     */
    RunnerViewController(QObject* parent, QTreeView* view);

    /*!
     * Destroys this runner view controller.
     */
    ~RunnerViewController();

    /*!
     * Returns the model that contains the data that is available
     * through the proxy model.
     */
    RunnerModel* runnerModel() const;

    /*!
     * Returns the proxy model that the view is presenting.
     */
    RunnerProxyModel* runnerProxyModel() const;

public slots:

    /*!
     * Selects all items in the view.
     */
    void selectAll() const;

    /*!
     * Unselects all items in the view.
     */
    void unselectAll() const;

private: // Operations

    /*!
     * If \a select is true then all items in the view selected,
     * otherwise unselected.
     */
    void selectAllItems(bool select) const;

    /*!
     * Handles key press events for the view. A space mimicks a mouse
     * click. +/- expand/collapse branches.
     */
    bool eventFilter(QObject* obj, QEvent* event);

    // Copy and assignment not supported.
    RunnerViewController(const RunnerViewController&);
    RunnerViewController& operator=(const RunnerViewController&);
};

} // namespace

#endif // VERITAS_RUNNERVIEWCONTROLLER_H
