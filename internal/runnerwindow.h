/* KDevelop xUnit plugin
 *
 * Copyright 2006 Ernst Huber <qxrunner@systest.ch>
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

#ifndef VERITAS_RUNNERWINDOW_H
#define VERITAS_RUNNERWINDOW_H

#include <QtGui/QLabel>
#include <QTreeView>
#include <QList>
#include <QTime>
#include <QProgressBar>
#include <KUrl>
#include "../projectselection.h"
#include "../veritasexport.h"

namespace KDevelop { class IProject; }
namespace Ui { class RunnerWindow;}

class QAction;
class QItemSelection;
class QAction;
class QProgressBar;
class KSelectAction;

namespace Veritas
{
class RunnerModel;
class RunnerProxyModel;
class ResultsModel;
class ResultsProxyModel;
class SelectionManager;
class VerboseToggle;
class ToSourceToggle;
class ResultsWidget;
class OverlayManager;
class TestExecutor;
class Test;
class TestBar;

/*!
 * \brief The RunnerWindow class defines the Veritas main toolwindow
 *
 * The RunnerWindow class presents a RunnerModel and its ResultsModel
 * in appropriate views to the user and provides commands for their
 * manipulation. Runner items can be executed.
 */
// TODO this is starting to smell like a GOD class. fix it
class VERITAS_EXPORT RunnerWindow : public QWidget
{
Q_OBJECT
public: // Operations

    /*! Creates a new RunnerWindow.
     * The IProjectFilter, if set, will be used by the ProjectSelection in the
     * toolbar.
     */
    explicit RunnerWindow(ResultsModel*, ProjectSelection::IProjectFilter* projectFilter = 0,
                          QWidget* parent = 0, Qt::WFlags flags = 0);

    /*!\note
     * Deleting the model provided for the main window is left to
     * the owner of the model instance. */
    ~RunnerWindow();

    /*! Sets the RunnerModel which holds the test-tree
     *  @note takes ownership. on succeeding calls the previous model will
     *  be deleted */
    void setModel(RunnerModel* model);

    /*! Accessors for Runner model & view.
     * The runner shows the test suite structure as
     * a tree. It allows for individual test selection. */
    QTreeView* runnerView() const;
    RunnerModel* runnerModel() const;
    RunnerProxyModel* runnerProxyModel() const;

    /*! Accessors for the Results model & view.
     * The result component shows test failure
     * information. */
    QTreeView* resultsView() const;
    ResultsModel* resultsModel() const;
    ResultsProxyModel* resultsProxyModel() const;

    const Ui::RunnerWindow* ui() const;

    QWidget* resultsWidget() const;

    void resetProgressBar() const;
    KDevelop::IProject* selectedProject() const;

Q_SIGNALS:
    void runCompleted() const;
    void requestReload() const;
    void requestReset() const;

public Q_SLOTS:

    void showVerboseTestOutput();
    void openTestSource();

private Q_SLOTS:

    // Bunch of slots to trigger status updates in the GUI
    void displayCompleted() const;
    void displayProgress(int numItems) const;
    void updateNumTotal(int numItems);
    void updateNumSelected(int numItems);
    void displayNumCompleted(int numItems);
    void displayNumErrors(int numItems) const;
    void displayNumFatals(int numItems) const;
    void displayNumExceptions(int numItems) const;

    /*! Filter all results which belong to the selected in the
     * results view. */
    void syncResultWithTest(const QItemSelection& selected,
                                  const QItemSelection& deselected) const;

    /*! Highlights the runner view row that corresponds to the results
     * proxy model selection in \a selected. \a deselected is ignored. */
    void syncTestWithResult(const QItemSelection& selected,
                                  const QItemSelection& deselected) const;

    /*! Ensures that the highlighted row in every view is visible. */
    void scrollToHighlightedRows() const;

    /*! Exectues the items in the model. */
    void runItems();

    /*! Stops test execution. */
    void stopItems();

    void jumpToSource(const QItemSelection& selected, const QItemSelection& deselected);
    void selectAll();
    void unselectAll();

    /*! If test with index @p i is currently expanded then collapse it.
        Vice versa if it is currently collapsed. */
    void expandOrCollapse(const QModelIndex& i) const;

    /*! Sets the project to show its tests.
     * The project can be a null pointer, which cleans the shown tests.
     *
     * @param project The project to set as the selected one.
     */
    void setSelectedProject(KDevelop::IProject* project);


private: // Operations

    // helpers for RunnerWindow(...) ctor
    void initItemStatistics();
    void connectActions();
    void connectFocusStuff();

    /*! Adds a ProjectSelection with the given projectFilter to the toolbar.
     *
     * @param projectFilter The IProjectFilter to use.
     */
    void addProjectMenu(ProjectSelection::IProjectFilter* projectFilter);

    // helpers for setModel(RunnerModel*)
    void stopPreviousModel();
    void connectItemStatistics(RunnerModel* model);
    void initProxyModels(RunnerModel* model);
    void connectProgressIndicators(RunnerModel* model);

    // the rest

    /*! Disables and modifies widgets and signals before running items. */
    void disableControlsBeforeRunning();

    /*! Enables and modifies widgets and signals after item execution
     * has finished. */
    void enableControlsAfterRunning() const;

    /*! If \a enable is true the actions for item manipulation are
     * enabled, otherwise disabled. */
    void enableItemActions(bool enable) const;

    /*! If \a enable is true syncResultWithTest() is enabled,
     * otherwise disabled. */
    void enableTestSync(bool enable) const;

    /*! If \a enable is true syncTestWithResult() is enabled,
     * otherwise disabled. */
    void enableResultSync(bool enable) const;

    /*! Sets \a numItems as text in \a labelForText. If \a numItems
     * is 0 the labels are hidden, otherwise made visible. */
    void displayStatusNum(QLabel* labelForText,
                          QLabel* labelForPic, int numItems) const;

    void enableToSource() const;

    // Copy and assignment not supported.
    RunnerWindow(const RunnerWindow&);
    RunnerWindow& operator=(const RunnerWindow&);
    void updateRunText() const;
    void updateSelectedText() const;

    TestBar* progressBar() const;

private:
    Ui::RunnerWindow *m_ui;            // QtDesigner main object
    ResultsWidget* m_results;
    OverlayManager* m_verbose;
    ProjectSelection* m_projectSelection;     // a dropdown box to select the 'current' project
    QMap<QString, QAction*> m_project2action;
    QTime m_stopWatch;                 // times test-runs, shown in the gui
    TestExecutor* m_executor;
    mutable bool m_isRunning;
    VerboseToggle* m_verboseToggle;
    ToSourceToggle* m_toSourceToggle;
    OverlayManager* m_toSource;
    KDevelop::IProject* m_currentProject;
};

class TestBar : public QProgressBar
{
public:
    explicit TestBar(QWidget* parent);
    virtual ~TestBar();
    void turnGreen();
    void turnRed();
};

} // namespace

#endif // VERITAS_RUNNERWINDOW_H
