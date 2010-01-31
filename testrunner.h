/*
 * This file is part of KDevelop
 * Copyright 2008 Manuel Breugelmans <mbr.nxi@gmail.com>
 * Copyright 2010 Daniel Calviño Sánchez <danxuliu@gmail.com>
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

#ifndef VERITAS_ITESTRUNNER_H
#define VERITAS_ITESTRUNNER_H

#include <interfaces/iplugin.h>
#include "projectselection.h"
#include "veritasexport.h"

class QWidget;
namespace KDevelop { class IProject; }
namespace Sublime { class View; }

namespace Veritas
{
class Test;
class ITestFramework;
class ITestTreeBuilder;

/*! This Test runner combines 2 toolviews:
 *  (i)  a runner-toolview which contains the test-tree
 *  (ii) a results-toolview that holds test status information,
 *       for example failure message, location and so on.
 *
 * TestRunner objects belong to an ITestFramework (plugin). */
class VERITAS_EXPORT TestRunner : public QObject
{
Q_OBJECT
public:

    /**
     * Creates a new TestRunner.
     * The IProjectFilter, if set, will be used in the runner toolview to prevent
     * the user from selecting projects that don't meet the conditions to be
     * used with this runner.
     */
    TestRunner(ITestFramework*, ITestTreeBuilder*,
               ProjectSelection::IProjectFilter* projectFilter = 0);
    virtual ~TestRunner();

    /*! Create a new test runner widget
        Call this in your toolview factory's create() member. */
    QWidget* runnerWidget();

    /*! The associated test results widget
        This shows assertion failures with source location */
    QWidget* resultsWidget();

public Q_SLOTS:
    void setupToolView(Veritas::Test*);

protected:
    KDevelop::IProject* project() const;

private Q_SLOTS:
    void removeResultsView();

    /*! Reloads the test tree for the selected project.
     */
    void reloadTree();

    /*! Sets an empty test tree.
     */
    void resetTree();

private:
    void spawnResultsView();

    class Private;
    Private* const d;
};

}

#endif // VERITAS_ITESTRUNNER_H
