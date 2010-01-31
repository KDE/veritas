/* KDevelop xUnit plugin
 *
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

#include "projectselectiontest.h"

#define protected public
#define private public
#include "../projectselection.h"
#undef private
#undef protected

#include "testutils.h"

#include <QAbstractItemView>
#include <QLayout>
#include <QToolBar>
#include <QtTest/QTestKeyClicksEvent>

#include <KComboBox>
#include <KConfigGroup>
#include <KStandardDirs>

#include <interfaces/iproject.h>
#include <interfaces/iprojectcontroller.h>
#include <shell/core.h>
#include <tests/autotestshell.h>

using KDevelop::Core;
using KDevelop::IProject;

using Veritas::ProjectSelection;
using Veritas::ProjectSelectionTest;

Q_DECLARE_METATYPE(KDevelop::IProject*)

/**
 * Modified version of private KSignalSpy found in
 * kdelibs/kdecore/util/qtest_kde.cpp.
 * Original KDESignalSpy, accessed through
 * QTest::kWaitForSignal(QObject*, const char*, int), can miss a signal if it is
 * emitted too quickly (that is, before the connect is reached). This modified
 * version, instead of starting the wait in the constructor, has a specific
 * method for it. So the object can be created before executing the call that
 * emits the signal, enabling it to register the signal before starting to wait
 * and thus ensuring that no signal will be missed.
 *
 * Moreover, this modified version allows to wait for up to n emitted signals
 * instead of only the first one. Note that the count isn't reseted after
 * receiving all the expected signals, so if you wait for more signals use an
 * absolute value (that is, the already received signals plus those you want to
 * wait for now).
 */
class SignalSpy: public QObject
{
Q_OBJECT
public:
    SignalSpy(QObject* object, const char* signal):
        QObject(0), m_signalSpy(object, signal), m_expectedCount(0)
    {
        connect(object, signal, this, SLOT(signalEmitted()));
    }

    bool waitForCount(int expectedCount, int timeout)
    {
        if (m_signalSpy.count() < expectedCount) {
            m_expectedCount = expectedCount;

            if (timeout > 0) {
                QObject::connect(&m_timer, SIGNAL(timeout()), &m_loop, SLOT(quit()));
                m_timer.setSingleShot(true);
                m_timer.start(timeout);
            }
            m_loop.exec();
        }

        if (m_signalSpy.count() >= expectedCount) {
            return true;
        }
        return false;
    }

private Q_SLOTS:
    void signalEmitted()
    {
        if (m_signalSpy.count() >= m_expectedCount) {
            m_timer.stop();
            m_loop.quit();
        }
    }
private:
    QSignalSpy m_signalSpy;
    int m_expectedCount;
    QEventLoop m_loop;
    QTimer m_timer;
};

class TestFilter: public ProjectSelection::IProjectFilter
{
public:

    virtual bool canBeSelected(KDevelop::IProject* project)
    {
        KSharedPtr<KSharedConfig> config = project->projectConfiguration();
        return config->group("Project").readEntry("Selectable", true);
    }

};

void ProjectSelectionTest::initTestCase()
{
    AutoTestShell::init();
    Core::initialize();
    qRegisterMetaType<KDevelop::IProject*>();

    m_testPath = KStandardDirs().saveLocation("tmp", "projectSelectionTest/");

    m_project1Url = writeProjectConfiguration("Project1", true);
    m_project2Url = writeProjectConfiguration("Project2", true);
    m_project3FilteredUrl = writeProjectConfiguration("Project3Filtered", false);
}

void ProjectSelectionTest::cleanup()
{
    SignalSpy projectClosedSignal(Core::self()->projectController(), SIGNAL(projectClosed(KDevelop::IProject*)));
    int count = Core::self()->projectController()->projects().count();

    foreach(IProject* project, Core::self()->projectController()->projects()) {
        Core::self()->projectController()->closeProject(project);
    }

    projectClosedSignal.waitForCount(count, 30000);

    //Project configuration file isn't the kdev4 project file used to load the
    //project, so if the configuration was modified in a test it would be
    //modified in the other tests if these files aren't removed
    QFile(m_testPath + ".kdev4/Project1.kdev4").remove();
    QFile(m_testPath + ".kdev4/Project2.kdev4").remove();
    QFile(m_testPath + ".kdev4/Project3Filtered.kdev4").remove();
}

void ProjectSelectionTest::cleanupTestCase()
{
    QDir().rmdir(m_testPath + ".kdev4/");
    QFile(m_project1Url.path()).remove();
    QFile(m_project2Url.path()).remove();
    QFile(m_project3FilteredUrl.path()).remove();
    QDir().rmdir(m_testPath);
}

void ProjectSelectionTest::testConstructor()
{
    ProjectSelection projectSelection(this);

    QCOMPARE(projectSelection.items().size(), 0);
}

void ProjectSelectionTest::testConstructorProjectsAlreadyOpened()
{
    openProject(m_project1Url);
    openProject(m_project2Url);
    openProject(m_project3FilteredUrl);

    ProjectSelection projectSelection(this);

    QCOMPARE(projectSelection.items().size(), 3);
    QVERIFY(projectSelection.action("Project1"));
    QVERIFY(projectSelection.action("Project1")->isEnabled());
    QVERIFY(projectSelection.action("Project2"));
    QVERIFY(projectSelection.action("Project2")->isEnabled());
    QVERIFY(projectSelection.action("Project3Filtered"));
    QVERIFY(projectSelection.action("Project3Filtered")->isEnabled());
}

void ProjectSelectionTest::testConstructorWithFilterProjectsAlreadyOpened()
{
    openProject(m_project1Url);
    openProject(m_project2Url);
    openProject(m_project3FilteredUrl);

    ProjectSelection projectSelection(this, new TestFilter());

    QCOMPARE(projectSelection.items().size(), 3);
    QVERIFY(projectSelection.action("Project1"));
    QVERIFY(projectSelection.action("Project1")->isEnabled());
    QVERIFY(projectSelection.action("Project2"));
    QVERIFY(projectSelection.action("Project2")->isEnabled());
    QVERIFY(projectSelection.action("Project3Filtered"));
    QVERIFY(!projectSelection.action("Project3Filtered")->isEnabled());
}

void ProjectSelectionTest::testProjectOpened()
{
    ProjectSelection projectSelection(this);

    openProject(m_project2Url);

    QCOMPARE(projectSelection.items().size(), 1);
    QVERIFY(projectSelection.action("Project2"));
    QVERIFY(projectSelection.action("Project2")->isEnabled());
    QVERIFY(!projectSelection.currentAction());

    openProject(m_project3FilteredUrl);

    QCOMPARE(projectSelection.items().size(), 2);
    QVERIFY(projectSelection.action("Project2"));
    QVERIFY(projectSelection.action("Project2")->isEnabled());
    QVERIFY(projectSelection.action("Project3Filtered"));
    QVERIFY(projectSelection.action("Project3Filtered")->isEnabled());
    QVERIFY(!projectSelection.currentAction());
}

void ProjectSelectionTest::testProjectOpenedFilterSet()
{
    ProjectSelection projectSelection(this, new TestFilter());

    openProject(m_project2Url);

    QCOMPARE(projectSelection.items().size(), 1);
    QVERIFY(projectSelection.action("Project2"));
    QVERIFY(projectSelection.action("Project2")->isEnabled());
    QVERIFY(!projectSelection.currentAction());

    openProject(m_project3FilteredUrl);

    QCOMPARE(projectSelection.items().size(), 2);
    QVERIFY(projectSelection.action("Project2"));
    QVERIFY(projectSelection.action("Project2")->isEnabled());
    QVERIFY(projectSelection.action("Project3Filtered"));
    QVERIFY(!projectSelection.action("Project3Filtered")->isEnabled());
    QVERIFY(!projectSelection.currentAction());
}

//Emitting IProjectcontroller::projectConfigurationChanged(KDevelop::IProject* project)
// is too complex. ProjectSelection::updateProject is called directly instead
// (it is connected to that signal)
void ProjectSelectionTest::testProjectConfigurationChanged()
{
    ProjectSelection projectSelection(this);

    openProject(m_project2Url);
    openProject(m_project3FilteredUrl);

    IProject* project2 = getProject("Project2");
    KSharedPtr<KSharedConfig> config = project2->projectConfiguration();
    config->group("Project").writeEntry("Selectable", false);
    projectSelection.updateProject(project2);

    QCOMPARE(projectSelection.items().size(), 2);
    QVERIFY(projectSelection.action("Project2"));
    QVERIFY(projectSelection.action("Project2")->isEnabled());
    QVERIFY(projectSelection.action("Project3Filtered"));
    QVERIFY(projectSelection.action("Project3Filtered")->isEnabled());
    QVERIFY(!projectSelection.currentAction());
}

void ProjectSelectionTest::testProjectConfigurationChangedFilterSet()
{
    ProjectSelection projectSelection(this, new TestFilter());

    openProject(m_project2Url);
    openProject(m_project3FilteredUrl);

    IProject* project2 = getProject("Project2");
    KSharedPtr<KSharedConfig> config = project2->projectConfiguration();
    config->group("Project").writeEntry("Selectable", false);
    projectSelection.updateProject(project2);

    QCOMPARE(projectSelection.items().size(), 2);
    QVERIFY(projectSelection.action("Project2"));
    QVERIFY(!projectSelection.action("Project2")->isEnabled());
    QVERIFY(projectSelection.action("Project3Filtered"));
    QVERIFY(!projectSelection.action("Project3Filtered")->isEnabled());
    QVERIFY(!projectSelection.currentAction());

    IProject* project3 = getProject("Project3Filtered");
    config = project3->projectConfiguration();
    config->group("Project").writeEntry("Selectable", true);
    projectSelection.updateProject(project3);

    QCOMPARE(projectSelection.items().size(), 2);
    QVERIFY(projectSelection.action("Project2"));
    QVERIFY(!projectSelection.action("Project2")->isEnabled());
    QVERIFY(projectSelection.action("Project3Filtered"));
    QVERIFY(projectSelection.action("Project3Filtered")->isEnabled());
    QVERIFY(!projectSelection.currentAction());
}

void ProjectSelectionTest::testProjectConfigurationChangedSelectedProject()
{
    ProjectSelection projectSelection(this);

    openProject(m_project1Url);
    openProject(m_project2Url);

    IProject* project1 = getProject("Project1");
    projectSelection.selectProject(project1);

    QSignalSpy projectSelectedSignal(&projectSelection, SIGNAL(projectSelected(KDevelop::IProject*)));

    KSharedPtr<KSharedConfig> config = project1->projectConfiguration();
    config->group("Project").writeEntry("Selectable", false);
    projectSelection.updateProject(project1);

    QCOMPARE(projectSelection.items().size(), 2);
    QVERIFY(projectSelection.action("Project1"));
    QVERIFY(projectSelection.action("Project1")->isEnabled());
    QVERIFY(projectSelection.action("Project2"));
    QVERIFY(projectSelection.action("Project2")->isEnabled());
    QCOMPARE(projectSelection.currentAction(), projectSelection.action("Project1"));
    QCOMPARE(projectSelectedSignal.count(), 0);
}

void ProjectSelectionTest::testProjectConfigurationChangedSelectedProjectFilterSet()
{
    ProjectSelection projectSelection(this, new TestFilter());

    openProject(m_project1Url);
    openProject(m_project2Url);

    IProject* project1 = getProject("Project1");
    projectSelection.selectProject(project1);

    QSignalSpy projectSelectedSignal(&projectSelection, SIGNAL(projectSelected(KDevelop::IProject*)));

    KSharedPtr<KSharedConfig> config = project1->projectConfiguration();
    config->group("Project").writeEntry("Selectable", false);
    projectSelection.updateProject(project1);

    QCOMPARE(projectSelection.items().size(), 2);
    QVERIFY(projectSelection.action("Project1"));
    QVERIFY(!projectSelection.action("Project1")->isEnabled());
    QVERIFY(projectSelection.action("Project2"));
    QVERIFY(projectSelection.action("Project2")->isEnabled());
    //In KDE 4.3/Qt 4.5.1 it seems that setting a null action in KSelectAction
    //unchecks the current action, although it is still returned by
    //currentAction()
    QVERIFY(!projectSelection.currentAction() || !projectSelection.currentAction()->isChecked());
    QCOMPARE(projectSelectedSignal.count(), 1);
    QVariant argument = projectSelectedSignal.at(0).at(0);
    QCOMPARE(qvariant_cast<IProject*>(argument), (IProject*)0);
}

void ProjectSelectionTest::testProjectClosed()
{
    ProjectSelection projectSelection(this);

    openProject(m_project1Url);
    openProject(m_project2Url);

    QSignalSpy projectSelectedSignal(&projectSelection, SIGNAL(projectSelected(KDevelop::IProject*)));

    IProject* project2 = getProject("Project2");
    closeProject(project2);

    QCOMPARE(projectSelection.items().size(), 1);
    QVERIFY(projectSelection.action("Project1"));
    QVERIFY(projectSelection.action("Project1")->isEnabled());
    QVERIFY(!projectSelection.currentAction());
    QCOMPARE(projectSelectedSignal.count(), 0);

    IProject* project1 = getProject("Project1");
    closeProject(project1);

    QCOMPARE(projectSelection.items().size(), 0);
    QVERIFY(!projectSelection.currentAction());
    QCOMPARE(projectSelectedSignal.count(), 0);
}

void ProjectSelectionTest::testProjectClosedSelectedProject()
{
    ProjectSelection projectSelection(this);

    openProject(m_project1Url);
    openProject(m_project2Url);

    QSignalSpy projectSelectedSignal(&projectSelection, SIGNAL(projectSelected(KDevelop::IProject*)));

    IProject* project2 = getProject("Project2");
    projectSelection.selectProject(project2);
    closeProject(project2);

    QCOMPARE(projectSelection.items().size(), 1);
    QVERIFY(projectSelection.action("Project1"));
    QVERIFY(projectSelection.action("Project1")->isEnabled());
    QVERIFY(!projectSelection.currentAction());
    QCOMPARE(projectSelectedSignal.count(), 1);
    QVariant argument = projectSelectedSignal.at(0).at(0);
    QCOMPARE(qvariant_cast<IProject*>(argument), (IProject*)0);

    IProject* project1 = getProject("Project1");
    projectSelection.selectProject(project1);
    closeProject(project1);

    QCOMPARE(projectSelection.items().size(), 0);
    QVERIFY(!projectSelection.currentAction());
    QCOMPARE(projectSelectedSignal.count(), 2);
    argument = projectSelectedSignal.at(1).at(0);
    QCOMPARE(qvariant_cast<IProject*>(argument), (IProject*)0);
}

//I haven't found a way to test ProjectSelection in MenuMode. In that mode,
//QTest::mousePressed, QTest::keyClick with space key or
//QToolButton::showMenu methods don't return until the pop up menu is
//closed (Qt 4.5.1/KDE 4.3), so there is no way (that I know) to select an
//action once the menu is shown (or even closing the menu) without real user
//interaction.
//Testing with a combo box on a tool bar isn't ideal, but it is better than no
//test at all.
void ProjectSelectionTest::testProjectSelectedByUserComboBoxModeOnToolBar()
{
    ProjectSelection* projectSelection = new ProjectSelection(this);
    projectSelection->setToolBarMode(KSelectAction::ComboBoxMode);

    openProject(m_project1Url);
    openProject(m_project2Url);

    QSignalSpy projectSelectedSignal(projectSelection, SIGNAL(projectSelected(KDevelop::IProject*)));

    QToolBar toolBar;
    toolBar.addAction(projectSelection);
    toolBar.show();

    KComboBox* projectSelectionWidget = qobject_cast<KComboBox*>(toolBar.widgetForAction(projectSelection));
    QVERIFY2(projectSelectionWidget, "Fix test, as ComboBoxMode widget seems to be no longer a KComboBox");
    projectSelectionWidget->setFocus();
    QTest::keyClick(projectSelectionWidget, Qt::Key_Space);
    //Key clicks are sent to the popup once it has been shown, not to the
    //ComboBox. Up and down keys on a ComboBox select the items, while up and
    //down keys on the popup only highlights them, and pressing enter on the
    //desired item is needed to select it.
    QTest::keyClick(projectSelectionWidget->view(), Qt::Key_Space);
    QTest::keyClick(projectSelectionWidget->view(), Qt::Key_Down);
    QTest::keyClick(projectSelectionWidget->view(), Qt::Key_Enter);

    QCOMPARE(projectSelection->currentAction(), projectSelection->action("Project2"));
    QCOMPARE(projectSelectedSignal.count(), 1);
    QVariant argument = projectSelectedSignal.at(0).at(0);
    QCOMPARE(qvariant_cast<IProject*>(argument), getProject("Project2"));
}

void ProjectSelectionTest::testProjectSelectedByUserComboBoxModeOnWidget()
{
    ProjectSelection* projectSelection = new ProjectSelection(this);
    projectSelection->setToolBarMode(KSelectAction::ComboBoxMode);

    openProject(m_project1Url);
    openProject(m_project2Url);

    QSignalSpy projectSelectedSignal(projectSelection, SIGNAL(projectSelected(KDevelop::IProject*)));

    QWidget parent;
    parent.setLayout(new QHBoxLayout());
    KComboBox* projectSelectionWidget = qobject_cast<KComboBox*>(projectSelection->requestWidget(&parent));
    parent.layout()->addWidget(projectSelectionWidget);
    parent.show();

    QVERIFY2(projectSelectionWidget, "Fix test, as ComboBoxMode widget seems to be no longer a KComboBox");
    projectSelectionWidget->setFocus();
    QTest::keyClick(projectSelectionWidget, Qt::Key_Space);
    //Key clicks are sent to the popup once it has been shown, not to the
    //ComboBox. Up and down keys on a ComboBox select the items, while up and
    //down keys on the popup only highlights them, and pressing enter on the
    //desired item is needed to select it.
    QTest::keyClick(projectSelectionWidget->view(), Qt::Key_Space);
    QTest::keyClick(projectSelectionWidget->view(), Qt::Key_Down);
    QTest::keyClick(projectSelectionWidget->view(), Qt::Key_Enter);

    QCOMPARE(projectSelection->currentAction(), projectSelection->action("Project2"));
    QCOMPARE(projectSelectedSignal.count(), 1);
    QVariant argument = projectSelectedSignal.at(0).at(0);
    QCOMPARE(qvariant_cast<IProject*>(argument), getProject("Project2"));
}

void ProjectSelectionTest::testSelectProject()
{
    ProjectSelection projectSelection(this);

    openProject(m_project1Url);
    openProject(m_project2Url);

    QSignalSpy projectSelectedSignal(&projectSelection, SIGNAL(projectSelected(KDevelop::IProject*)));

    IProject* project2 = getProject("Project2");

    QVERIFY(projectSelection.selectProject(project2));
    QCOMPARE(projectSelection.currentAction(), projectSelection.action("Project2"));
    QCOMPARE(projectSelectedSignal.count(), 0);
}

void ProjectSelectionTest::testSelectProjectNullPointer()
{
    ProjectSelection projectSelection(this);

    openProject(m_project1Url);
    openProject(m_project2Url);

    QSignalSpy projectSelectedSignal(&projectSelection, SIGNAL(projectSelected(KDevelop::IProject*)));

    IProject* project2 = getProject("Project2");
    projectSelection.selectProject(project2);

    QVERIFY(projectSelection.selectProject(0));
    //In KDE 4.3/Qt 4.5.1 it seems that setting a null action in KSelectAction
    //unchecks the current action, although it is still returned by
    //currentAction()
    QVERIFY(!projectSelection.currentAction() || !projectSelection.currentAction()->isChecked());
    QCOMPARE(projectSelectedSignal.count(), 0);
}

void ProjectSelectionTest::testSelectProjectFilteredProject()
{
    ProjectSelection projectSelection(this, new TestFilter());

    openProject(m_project2Url);
    openProject(m_project3FilteredUrl);

    QSignalSpy projectSelectedSignal(&projectSelection, SIGNAL(projectSelected(KDevelop::IProject*)));

    IProject* project3 = getProject("Project3Filtered");

    QVERIFY(!projectSelection.selectProject(project3));
    QVERIFY(!projectSelection.currentAction());
    QCOMPARE(projectSelectedSignal.count(), 0);
}

void ProjectSelectionTest::testSelectProjectUnknownProject()
{
    ProjectSelection projectSelection(this);

    openProject(m_project1Url);
    openProject(m_project2Url);

    QSignalSpy projectSelectedSignal(&projectSelection, SIGNAL(projectSelected(KDevelop::IProject*)));

    IProject* project2 = getProject("Project2");
    closeProject(project2);

    QVERIFY(!projectSelection.selectProject(project2));
    QVERIFY(!projectSelection.currentAction());
    QCOMPARE(projectSelectedSignal.count(), 0);
}

/////////////////////////////////// Helpers ////////////////////////////////////

//Modified from kdevplatform/shell/tests/projectcontrollertest.h

KUrl ProjectSelectionTest::writeProjectConfiguration(const QString& name,
                                                     bool selectable) const
{
    KUrl configurationUrl = KUrl(m_testPath + name + ".kdev4");

    QFile file(configurationUrl.pathOrUrl());
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << "[Project]\n"
        << "Name=" << name << "\n"
        << "Selectable=" << selectable << "\n";
    file.close();

    return configurationUrl;
}

void ProjectSelectionTest::openProject(const KUrl& url) const
{
    SignalSpy projectOpenedSignal(Core::self()->projectController(),
                                  SIGNAL(projectOpened(KDevelop::IProject*)));
    Core::self()->projectController()->openProject(url);
    if (!projectOpenedSignal.waitForCount(1, 30000)) {
        QWARN("Timeout waiting for projectOpened");
    }
}

IProject* ProjectSelectionTest::getProject(const QString& name) const
{
    return Core::self()->projectController()->findProjectByName(name);
}

void ProjectSelectionTest::closeProject(IProject* project) const
{
    SignalSpy projectClosedSignal(Core::self()->projectController(),
                                  SIGNAL(projectClosed(KDevelop::IProject*)));
    Core::self()->projectController()->closeProject(project);
    if (!projectClosedSignal.waitForCount(1, 30000)) {
        QWARN("Timeout waiting for projectClosed");
    }
}

QTEST_KDEMAIN(ProjectSelectionTest, GUI)

#include "moc_projectselectiontest.cpp"
#include "projectselectiontest.moc"
