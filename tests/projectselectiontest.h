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

#ifndef VERITAS_PROJECTSELECTIONTEST_H
#define VERITAS_PROJECTSELECTIONTEST_H

#include <QtCore/QObject>

#include <KUrl>

class QAction;

namespace KDevelop
{
class IProject;
}

namespace Veritas
{

class ProjectSelection;

class ProjectSelectionTest: public QObject
{
Q_OBJECT
private slots:

    void initTestCase();
    void cleanup();
    void cleanupTestCase();

    void testConstructor();
    void testConstructorProjectsAlreadyOpened();
    void testConstructorWithFilterProjectsAlreadyOpened();

    void testProjectOpened();
    void testProjectOpenedFilterSet();

    void testProjectConfigurationChanged();
    void testProjectConfigurationChangedFilterSet();
    void testProjectConfigurationChangedSelectedProject();
    void testProjectConfigurationChangedSelectedProjectFilterSet();

    void testProjectClosed();
    void testProjectClosedSelectedProject();

    void testProjectSelectedByUserComboBoxModeOnToolBar();
    void testProjectSelectedByUserComboBoxModeOnWidget();

    void testSelectProject();
    void testSelectProjectNullPointer();
    void testSelectProjectFilteredProject();
    void testSelectProjectUnknownProject();

private:

    KUrl writeProjectConfiguration(const QString& name, bool selectable) const;
    void openProject(const KUrl& url) const;
    KDevelop::IProject* getProject(const QString& name) const;
    void closeProject(KDevelop::IProject* project) const;

    QString m_testPath;

    KUrl m_project1Url;
    KUrl m_project2Url;
    KUrl m_project3FilteredUrl;

};

}

#endif
