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

#include "projectselection.h"

#include <KLocale>

#include <interfaces/icore.h>
#include <interfaces/iproject.h>
#include <interfaces/iprojectcontroller.h>

using KDevelop::ICore;
using KDevelop::IProject;
using KDevelop::IProjectController;

using Veritas::ProjectSelection;

namespace Veritas
{

class IProjectFilterPrivate
{
public:

    IProjectFilterPrivate(const QString& explanation):
        explanation(explanation)
    {
    }

    QString explanation;

};

}

using Veritas::IProjectFilterPrivate;

ProjectSelection::IProjectFilter::IProjectFilter(const QString& explanation /*= QString()*/):
    d(new IProjectFilterPrivate(explanation))
{
}

ProjectSelection::IProjectFilter::~IProjectFilter()
{
    delete d;
}

QString ProjectSelection::IProjectFilter::explanation() const
{
    return d->explanation;
}



namespace Veritas
{

class ProjectSelectionPrivate
{
public:

    ProjectSelectionPrivate(ProjectSelection::IProjectFilter* projectFilter):
        projectFilter(projectFilter)
    {
    }

    ProjectSelection::IProjectFilter* projectFilter;

    QHash<const KDevelop::IProject*, QAction*> actionForProject;

    QHash<const QAction*, KDevelop::IProject*> projectForAction;

};

}

using Veritas::ProjectSelectionPrivate;

ProjectSelection::ProjectSelection(QObject* parent /*= 0*/, IProjectFilter* projectFilter /*= 0*/):
    KSelectAction(i18nc("@action:button Shows a popup menu to select a project", "Project"), parent),
    d(new ProjectSelectionPrivate(projectFilter))
{
    setToolTip(i18nc("@info:tooltip", "Select project"));

    QString whatsThis = i18nc("@info:whatsthis", "<para>Selector for opened projects.</para>\
<para>It shows a list with all the currently opened projects to select one of them.</para>");
    if (projectFilter) {
        whatsThis += i18nc("@info:whatsthis Appended to the end of another WhatsThis help",
                           "<para>Under some circumstances, some of the projects may appear \
as disabled. This happens when a project does not meet the conditions to be selected (for \
example, some necessary configuration is not set).</para>");
        if (!projectFilter->explanation().isEmpty()) {
            whatsThis += projectFilter->explanation();
        }
    }
    setWhatsThis(whatsThis);

    //Under normal circumstances there will be a Core and a ProjectController.
    //The constructor will return here only in the tests of classes that use
    //ProjectSelection that don't set a full environment.
    if (!ICore::self() || !ICore::self()->projectController()) {
        return;
    }

    IProjectController* projectController = ICore::self()->projectController();
    foreach(IProject* project, projectController->projects()) {
        addProject(project);
    }

    connect(projectController, SIGNAL(projectOpened(KDevelop::IProject*)),
            this, SLOT(addProject(KDevelop::IProject*)));
    connect(projectController, SIGNAL(projectConfigurationChanged(KDevelop::IProject*)),
            this, SLOT(updateProject(KDevelop::IProject*)));
    connect(projectController, SIGNAL(projectClosing(KDevelop::IProject*)),
            this, SLOT(removeProject(KDevelop::IProject*)));

    connect(this, SIGNAL(triggered(QAction*)),
            this, SLOT(selectAction(QAction*)));
}

ProjectSelection::~ProjectSelection()
{
    delete d->projectFilter;
    delete d;
}

QList<IProject*> ProjectSelection::projects() const
{
    return d->projectForAction.values();
}

bool ProjectSelection::selectProject(IProject* project)
{
    if (project == 0) {
        setCurrentItem(-1);
        return true;
    }

    QAction* action = d->actionForProject.value(project);
    return setCurrentAction(action);
}

void ProjectSelection::addProject(IProject* project)
{
    Q_ASSERT(project);
    Q_ASSERT(!d->actionForProject.contains(project));

    QAction* action = new QAction(project->name(), this);
    action->setCheckable(true);
    if (d->projectFilter && !d->projectFilter->canBeSelected(project)) {
        action->setDisabled(true);
    }
    addAction(action);

    d->actionForProject.insert(project, action);
    d->projectForAction.insert(action, project);
}

void ProjectSelection::updateProject(IProject* project)
{
    Q_ASSERT(project);
    Q_ASSERT(d->actionForProject.contains(project));

    QAction* action = d->actionForProject.value(project);

    if (d->projectFilter && !d->projectFilter->canBeSelected(project)) {
        action->setDisabled(true);
        if (action == currentAction()) {
            setCurrentItem(-1);
            emit projectSelected(0);
        }
    } else {
        action->setEnabled(true);
    }
}

void ProjectSelection::removeProject(IProject* project)
{
    Q_ASSERT(project);
    Q_ASSERT(d->actionForProject.contains(project));

    QAction* action = d->actionForProject.value(project);
    d->actionForProject.remove(project);
    d->projectForAction.remove(action);

    if (action == currentAction()) {
        emit projectSelected(0);
    }

    delete removeAction(action);
}

void ProjectSelection::selectAction(QAction* action)
{
    emit projectSelected(d->projectForAction.value(action));
}

#include "projectselection.moc"
