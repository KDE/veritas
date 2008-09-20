/* This file is part of KDevelop
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

#include "runnertesthelper.h"

#include <QModelIndex>
#include <QTimer>
#include <QAbstractItemModel>
#include <qtest_kde.h>
#include "kasserts.h"

#include "ui_runnerwindow.h"
#include "../mvc/resultsmodel.h"
#include "../mvc/runnermodel.h"
#include "../mvc/runnerwindow.h"
#include "../test.h"

using Veritas::RunnerTestHelper;
using Veritas::Test;

RunnerTestHelper::RunnerTestHelper()
    : m_window(0), m_show(false)
{}

void RunnerTestHelper::initializeGUI()
{
    QStringList resultHeaders;
    resultHeaders << i18n("Test Name") << i18n("Result") << i18n("Message")
                  << i18n("File Name") << i18n("Line Number");
    m_window = new RunnerWindow(new ResultsModel(resultHeaders));
}

void RunnerTestHelper::cleanupGUI()
{
    if (m_window) delete m_window;
}

void RunnerTestHelper::setRoot(Test* root)
{
    RunnerModel* model = new RunnerModel;
    model->setRootItem(root);
    m_window->setModel(model);
}

void RunnerTestHelper::verifyResultItems(QList<QStringList> expected)
{
    nrofMessagesEquals(expected.size());
    for (int i = 0; i < expected.size(); i++) {
        checkResultItem(i, expected.value(i));
    }
}

void RunnerTestHelper::checkResultItem(int num, const QStringList& item)
{
    QAbstractItemModel* results = m_window->resultsView()->model();

    KOMPARE(item[0], results->data(results->index(num, 0))); // test name
    KOMPARE(item[1], results->data(results->index(num, 1))); // failure message
    KOMPARE(item[2], results->data(results->index(num, 2))); // filename
    KOMPARE(item[3], results->data(results->index(num, 3))); // line number
}

void RunnerTestHelper::nrofMessagesEquals(int num)
{
    QAbstractItemModel* results = m_window->resultsView()->model();
    for (int i = 0; i < num; i++) {
        KVERIFY(results->index(i, 0).isValid());
    }
    KVERIFY(!results->index(num, 0).isValid());
}

void RunnerTestHelper::triggerRunAction()
{
    m_window->ui()->actionStart->trigger();
}

void RunnerTestHelper::setShowWidget(bool show)
{
    m_show = show;
}

void RunnerTestHelper::runTests()
{
    QTimer* t = new QTimer();
    t->setSingleShot(true);
    t->setInterval(10);
    connect(t, SIGNAL(timeout()), this, SLOT(triggerRunAction()));
    t->start();

    if (m_show) {
        m_window->show();
        QTest::qWait(5000);
    }

    bool gotSignal = QTest::kWaitForSignal(
        m_window->runnerModel(),
        SIGNAL(allItemsCompleted()),
        2000);
    QVERIFY2(gotSignal, "Timeout while waiting for runner items to complete execution");
}

void RunnerTestHelper::verifyTestTree(QStringList runnerItems)
{
    foreach(QString s, runnerItems) {
        QStringList spl = s.split(" ");
        int lvl0 = spl[0].toInt();
        int lvl1 = (spl.size() > 2) ? spl[1].toInt() : -1;
        int lvl2 = (spl.size() > 3) ? spl[2].toInt() : -1;
        QVariant exp = (spl.last() == "x") ? QVariant() : spl.last();
        verifyTest(exp, lvl0, lvl1, lvl2);
    }
}

void RunnerTestHelper::verifyTest(const QVariant& expected, int lvl0, int lvl1, int lvl2)
{
    QAbstractItemModel* runner = m_window->ui()->treeRunner->model();
    QModelIndex index = runner->index(lvl0, 0);
    if (lvl1 != -1) {
        index = runner->index(lvl1, 0, index);
        if (lvl2 != -1) {
            index = runner->index(lvl2, 0, index);
        }
    }
    KOMPARE(expected, runner->data(index));
}

namespace {

QString stateToString(Veritas::TestState state)
{
    QString str;
    switch(state) {
    case Veritas::RunSuccess:
        str = "RunSuccess";
        break;
    case Veritas::RunError:
        str = "RunError";
        break;
    case Veritas::RunFatal:
        str = "RunFatal";
        break;
    case Veritas::NoResult:
        str = "NoResult";
        break;
    case Veritas::RunException:
        str = "RunException";
        break;
    }
    return str;
}

void verifyTestState(Veritas::Test* test, const QString path, QMap<QString, Veritas::TestState>& expectedStates)
{
    KVERIFY_MSG(expectedStates.contains(path), QString("No expected test-state provided for %1").arg(path));
    Veritas::TestState expected = expectedStates[path];
    expectedStates.remove(path);
    Veritas::TestResult* res = test->result();
    KVERIFY(res);
    QString failMsg = QString("Wrong teststate for %3. Expected %1 but got %2").arg(stateToString(expected)).arg(stateToString(res->state())).arg(test->name());
    KOMPARE_MSG(expected, res->state(), failMsg);

}

} // end anonymous namespace

void RunnerTestHelper::verifyTestStates(QMap<QString, Veritas::TestState> expectedState, Veritas::Test* root)
{
    // TestCases are on lvl3 in the test-tree. this is kinda weak ... fix it
    for(int i=0; i<root->childCount(); i++) {
        Veritas::Test* suite = root->child(i);
        for (int i=0; i<suite->childCount(); i++) {
            Veritas::Test* caze = suite->child(i);
            QString path = suite->name() + "/" + caze->name();
            if (expectedState.contains(path)) {
                verifyTestState(caze, path, expectedState);
            }
            for (int i=0; i<caze->childCount(); i++) {
                Veritas::Test* cmd = caze->child(i);
                QString path = suite->name() + "/" + caze->name() + "/" + cmd->name();
                verifyTestState(cmd, path, expectedState);
            }
        }
    }
    KVERIFY(expectedState.isEmpty());
}

QWidget* RunnerTestHelper::runnerWidget() const
{
    return m_window;
}

#include "runnertesthelper.moc"
