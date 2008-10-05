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

#ifndef VERITAS_MODELCREATION_H
#define VERITAS_MODELCREATION_H

#include "../internal/runnermodel.h"
#include "../internal/resultsmodel.h"
#include "../internal/test_p.h"
#include "../test.h"
#include <KDebug>

namespace Veritas
{

using Veritas::RunnerModel;
using Veritas::ResultsModel;

class TestStub : public Test
{
public:
    TestStub(const QList<QVariant>& data, Test* parent)
            : Test(data, parent), m_state(Veritas::RunSuccess), m_shouldRun(false), m_result(0) {}

    TestStub(const char* name, Test* parent)
            : Test(name, parent), m_state(Veritas::RunSuccess), m_shouldRun(true), m_result(0) {}

    bool shouldRun() const {
        return m_shouldRun;
    }

public slots:
    int run() {
        if (!child(0)) {  // Have nothing to do as a parent
            signalStarted();
            if (!m_result) {
                // do not interfere when result was set from the outside
                m_result = new TestResult;
                m_result->setState(m_state);
            }
            internal()->setData(1, QString::number(row()) + QString("_1"));
            internal()->setData(2, QString::number(row()) + QString("_2"));
            setResult(m_result);
            signalFinished();
        }
        executedItems.push_back(row());
        emit executionFinished();
        return state();
    }

public:
    static QList<int> executedItems; // store the index of rows that got executed
    TestState m_state;
    bool m_shouldRun;
    TestResult* m_result;
};

QList<int> TestStub::executedItems;

class RunnerModelStub : public RunnerModel
{
public:
    RunnerModelStub(bool fill = true)
            : RunnerModel(NULL),
            col0Caption("run_col0"),
            col1Caption("run_col1"),
            col2Caption("run_col2") {
        if (fill) {
            this->fill1();
        }
    }

    void fill1() {
        addParents();
        setRootItem(root);
    }

    void fill2() {
        addParents();
        appendSomeChildren();
        setRootItem(root);
    }

    void addParents() {
        QList<QVariant> rootData;
        rootData << col0Caption << col1Caption << col2Caption;
        root = new TestStub(rootData, 0);

        QList<QVariant> columnData;
        columnData << "00" << "01" << "02";
        item1 = new TestStub(columnData, root);
        TestResult* tr = new TestResult;
        tr->setMessage("01");
        tr->setFile(KUrl("02"));
        tr->setState(Veritas::RunSuccess);
        item1->setResult(tr);
        root->addChild(item1);

        columnData.clear();
        columnData << "10" << "11" << "12";
        item2 = new TestStub(columnData, root);
        tr = new TestResult;
        tr->setState(Veritas::RunFatal);
        tr->setMessage("11");
        tr->setFile(KUrl("12"));
        item2->setResult(tr);
        root->addChild(item2);
    }

    Test* fetchItem(const QModelIndex& index) {
        return static_cast<Test*>(index.internalPointer());
    }

    void decapitate() {
        setRootItem(0);
    }

    void appendSomeChildren() {
        child11 = new TestStub("child11", item1);
        item1->addChild(child11);
        child12 = new TestStub("child12", item1);
        item1->addChild(child12);
        child21 = new TestStub("child21", item2);
        item2->addChild(child21);
    }

    // column headers
    QVariant col0Caption;
    QVariant col1Caption;
    QVariant col2Caption;

    TestStub* root;
    TestStub* item1;
    TestStub* child11;
    TestStub* child12;
    TestStub* item2;
    TestStub* child21;
};

inline RunnerModelStub* createRunnerModelStub(bool fill = true)
{
    return new RunnerModelStub(fill);
}

class ResultsHeader
{
public:
    static const QString col0;
    static const QString col1;
    static const QString col2;
};

const QString ResultsHeader::col0 = "col0";
const QString ResultsHeader::col1 = "col1";
const QString ResultsHeader::col2 = "col2";

inline QPair<ResultsModel*,RunnerModelStub*> createResultsModelStub()
{
    QStringList header;
    header << ResultsHeader::col0 << ResultsHeader::col1 << ResultsHeader::col2;

    ResultsModel* model = new ResultsModel(header);
    RunnerModelStub* runnerModel = new RunnerModelStub();

    model->addResult(runnerModel->index(0, 0)); // invoke slot
    model->addResult(runnerModel->index(1, 0)); // invoke slot

    return qMakePair(model, runnerModel);
}

}


#endif // VERITAS_MODELCREATION_H
