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

#include "runnerwindowtest.h"

#include <KDebug>
#include <QRegExp>
#include "testutils.h"

#include "modelcreation.h"

#include "../internal/runnerproxymodel.h"
#include "../internal/runnerwindow.h"
#include "../internal/runnermodel.h"
#include "../internal/resultsproxymodel.h"
#include "../internal/test_p.h"

using Veritas::RunnerWindow;
using Veritas::Test;
using Veritas::ResultsModel;
using Veritas::ResultsProxyModel;

using Veritas::TestStub;
using Veritas::RunnerModelStub;
using Veritas::createRunnerModelStub;
using Veritas::RunnerWindowTest;

// fixture
void RunnerWindowTest::init()
{
    model = createRunnerModelStub(false);
    model->fill2();
    QStringList resultHeaders;
    resultHeaders << i18n("Test") << i18n("Message")
                  << i18n("File") << i18n("Line");
    m_resultsModel = new ResultsModel(resultHeaders);
    window = new RunnerWindow(m_resultsModel);
    window->setModel(model);
    //window->show();
    m_ui = window->ui();
    m_proxy = window->runnerProxyModel();
    m_view = window->runnerView();
    m_resultsProxy = window->resultsProxyModel();
    TestStub::executedItems.clear();
}

// fxiture
void RunnerWindowTest::cleanup()
{
    m_ui->actionExit->trigger();
    if (window) delete window;
    if (m_resultsModel) delete m_resultsModel;
}

// helper
void RunnerWindowTest::checkAllItems(checkMemberFun f)
{
    KVERIFY( (this->*f)(model->item1) );
    KVERIFY( (this->*f)(model->child11) );
    KVERIFY( (this->*f)(model->child12) );
    KVERIFY( (this->*f)(model->item2) );
    KVERIFY( (this->*f)(model->child21) );
}

// helper
bool RunnerWindowTest::isSelected(TestStub* item)
{
    return item->internal()->checkState() == Qt::Checked;
}

// helper
void RunnerWindowTest::selectSome()
{
    model->item1->internal()->setCheckState(Qt::Checked);
    model->child11->internal()->setCheckState(Qt::Checked);
    model->child12->internal()->setCheckState(Qt::Unchecked);
    model->item2->internal()->setCheckState(Qt::Unchecked);
    model->child21->internal()->setCheckState(Qt::Checked);
}

// command
void RunnerWindowTest::selectAll()
{
    selectSome();
    model->countItems();
    QTest::qWait(100);
    m_ui->actionSelectAll->trigger();
    QTest::qWait(100);
    checkAllItems(&RunnerWindowTest::isSelected);
}

// helper
bool RunnerWindowTest::isNotSelected(TestStub* item)
{
    return item->internal()->checkState() == Qt::Unchecked;
}

// command
void RunnerWindowTest::unselectAll()
{
    selectSome();
    m_ui->actionUnselectAll->trigger();
    QTest::qWait(100);
    checkAllItems(&RunnerWindowTest::isNotSelected);
}

// helper
bool RunnerWindowTest::isExpanded(TestStub* item)
{
    QModelIndex i = m_proxy->mapFromSource(item->internal()->index());
    return m_view->isExpanded(i);
}

// helper
void RunnerWindowTest::expandSome()
{
    m_view->expand(m_proxy->index(0,0));
}

// command
void RunnerWindowTest::expandAll()
{
    expandSome();
    m_ui->actionExpandAll->trigger();
    QTest::qWait(100);
    KVERIFY( isExpanded(model->item1) );
    KVERIFY( isExpanded(model->item2) );
}

// helper
bool RunnerWindowTest::isCollapsed(TestStub* item)
{
    QModelIndex i = m_proxy->mapFromSource(item->internal()->index());
    return !m_view->isExpanded(i);
}

// command
void RunnerWindowTest::collapseAll()
{
    expandSome();
    m_ui->actionCollapseAll->trigger();
    QTest::qWait(100);
    KVERIFY( isCollapsed(model->item1) );
    KVERIFY( isCollapsed(model->item2) );
}

// command
void RunnerWindowTest::startItems()
{
    runAllTests();
    window->show();

    // check they got indeed executed
    // the rows of items that got executed are stored
    // in the stub
    QCOMPARE(0, TestStub::executedItems.value(0));
    QCOMPARE(1, TestStub::executedItems.value(1));
    QCOMPARE(0, TestStub::executedItems.value(2));

    // validate the test content
    QCOMPARE(Veritas::RunSuccess, model->child11->state());
    QCOMPARE(Veritas::RunSuccess, model->child12->state());
    QCOMPARE(Veritas::RunSuccess, model->child21->state());
}

// command
void RunnerWindowTest::deselectItems()
{
    // select only one of the runner items
    // validate that the other one didn't get executed
    TDD_TODO;
}

// command
void RunnerWindowTest::newModel()
{
    // init() has loaded a model, now replace
    // it with another one, with different items
    RunnerModelStub* model = createRunnerModelStub(false);
    model->fill1();
    window->setModel(model);
    //window->show();

    // it should now contain 2 top level items without
    // children. since thats what model->fill1() does.
    RunnerModel* actual = window->runnerModel();
    KOMPARE(model, actual);
    QModelIndex c1 = actual->index(0,0);
    KVERIFY(c1.isValid());
    KOMPARE("00", actual->data(c1));
    KVERIFY(!c1.child(0,0).isValid());

    QModelIndex c2 = actual->index(1,0);
    KVERIFY(c2.isValid());
    KOMPARE("10", actual->data(c2));
    KVERIFY(!c2.child(0,0).isValid());

    KVERIFY(!actual->index(2,0).isValid());
}

// helper
void RunnerWindowTest::runAllTests()
{
    // invoke the run action
    m_ui->actionStart->trigger();
    //window->show();

    // de-comment the line below to inspect the window manually
    //QTest::qWait(5000);
}

// helper
void RunnerWindowTest::assertResultItemEquals(const QModelIndex& i, const QString& content)
{
    QTreeView* resv = window->resultsView();

    KOMPARE(content, m_resultsProxy->data(i, Qt::DisplayRole));
    KVERIFY(i.isValid());
    KVERIFY(!resv->isFirstColumnSpanned(i.row(),QModelIndex()));
}

// debug helper
void RunnerWindowTest::printModel(const QModelIndex& mi, int lvl)
{
    QModelIndex i = mi;
    char space[512];
    for (int j=0; j<2*lvl; j++) {
        space[j] = '+';
    }
    space[2*lvl] = 0x0;

    while (i.isValid()) {
        kDebug() << space << i.row()
                 << i.model()->data(i, Qt::DisplayRole).toString();
        if (i.child(0,0).isValid()) {
            printModel(i.child(0,0), lvl+1);
        }
        i = i.sibling(i.row()+1, 0);
    }
}

#define PRINT_MODEL(mdl) printModel(mdl->index(0,0), 0)

// command
void RunnerWindowTest::clickRunnerResults()
{
    model->child11->m_state = Veritas::RunSuccess;
    model->child12->m_state = Veritas::RunError;
    model->child21->m_state = Veritas::RunError;

    runAllTests();

    // r0
    //   -child11
    //   -child12 [failed]
    // r1
    //   -child21 [failed]

    // select r1 in the runner-tree
    // since child21 has failed this item's result should now be shown in
    // the resultsview. All other results are expected to be filtered
    QModelIndex i = m_proxy->index(1,0);
    m_view->selectionModel()->select(i, QItemSelectionModel::Select);
    assertResultsProxyShowsOnly("child21");

    // now select child12.
    // the results view should have been reset and show only child12's result item
    i = m_proxy->index(0,0).child(1,0);
    m_view->selectionModel()->select(i, QItemSelectionModel::Select);
    assertResultsProxyShowsOnly("child12");

    // now select child11
    // since this test did pass, the results view should be empty
    i = m_proxy->index(0,0).child(0,0);
    m_view->selectionModel()->select(i, QItemSelectionModel::Select);
    assertResultsProxyShowsNothing();
}

void RunnerWindowTest::assertResultsProxyShowsOnly(const QString& itemData)
{
    QModelIndex result = m_resultsProxy->index(0,0);
    KVERIFY_MSG(result.isValid(), 
        "Was expecting to find something in the resultsview, "
        "however it is empty (filtered).");
    KVERIFY_MSG(!m_resultsProxy->index(1,0).isValid(),
        "Resultsview should contain only a single item.");
    KOMPARE(itemData, m_resultsProxy->data(result));
}

void RunnerWindowTest::assertResultsProxyShowsNothing()
{
    QModelIndex first = m_resultsProxy->index(0,0);
    KVERIFY_MSG(!first.isValid(),
        QString("Was expecting an empty resultiew but index(0,0) is valid. data: %1").
            arg(m_resultsProxy->data(first).toString()));
}

// command
void RunnerWindowTest::progressBarMaxedOutAfterRun()
{
    runAllTests();
    QProgressBar* bar = window->ui()->progressRun;
    KOMPARE(bar->maximum(), bar->value());
}

// command
void RunnerWindowTest::infoResultShown()
{
    TestResult* res = new TestResult;
    res->setMessage("FooBar");
    res->setState(Veritas::RunInfo);
    model->child11->m_result = res;
    runAllTests();

    QModelIndex i = m_resultsProxy->index(0,0);
    KOMPARE("child11", i.data(Qt::DisplayRole));
    KOMPARE("FooBar", i.sibling(0,1).data(Qt::DisplayRole));
}

void RunnerWindowTest::testRanCaption()
{
    QRegExp r("Selected [0-9]+ tests of [0-9]+");
    KVERIFY_MSG(r.exactMatch(m_ui->labelRunText->text()), m_ui->labelRunText->text());

    runAllTests();

    r.setPattern("All tests completed in [0-9]*.[0-9]* seconds");
    KVERIFY_MSG(r.exactMatch(m_ui->labelRunText->text()), m_ui->labelRunText->text());
}

void RunnerWindowTest::testRanCaptionWithSingleTest()
{
    m_ui->actionUnselectAll->trigger();
    model->child12->internal()->setCheckState(Qt::Checked);
    //As Internal is modified directly, call countItems in the model to update
    //the count of selected items.
    model->countItems();

    QRegExp r("Selected 1 test of [0-9]+");
    KVERIFY_MSG(r.exactMatch(m_ui->labelRunText->text()), m_ui->labelRunText->text());

    m_ui->actionStart->trigger();

    r.setPattern("All tests completed in [0-9]*.[0-9]* seconds");
    KVERIFY_MSG(r.exactMatch(m_ui->labelRunText->text()), m_ui->labelRunText->text());
}

QTEST_KDEMAIN(RunnerWindowTest, GUI)
