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

#ifndef VERITAS_PROJECTSELECTION_H
#define VERITAS_PROJECTSELECTION_H

#include <KDE/KSelectAction>

#include "veritasexport.h"

namespace KDevelop
{
class IProject;
}

namespace Veritas
{

/**
 * KSelectAction for selecting one of the opened projects.
 * The list contains an item for each opened project, and there can be only one
 * project selected at a time.
 *
 * However, not every project may be selectable. Under some circumstances, a
 * project shouldn't be selected (for example, if something is missed in its
 * configuration), so the projects to be selected can be filtered. Note that the
 * filtered project will still be shown, but it will be disabled.
 *
 * A subclass of ProjectSelection::IProjectFilter must be implemented with the
 * desired filter behavior, and a new object of this class must be set in
 * ProjectSelection constructor. The ProjectSelection takes ownership of the
 * filter, so it is deleted when the ProjectSelection is destroyed.
 *
 * The ProjectSelection is updated whenever a project is opened, updated or
 * closed. If a project is opened, it is added to the ProjectSelection and
 * enabled or disabled depending on the filter. If an already opened project is
 * updated (its configuration changed) it is enabled or disabled depending on
 * the filter. If it is disabled and it was the currently selected project, it
 * will be deselected. If an already opened project is closed, it is removed
 * from the ProjectSelection. If it was the currently selected project, it will
 * be deselected.
 *
 * Whenever a project is selected by the user or deselected due to any of the
 * above circumstances, projectSelected(KDevelop::IProject*) signal is emitted.
 *
 * The ProjectSelection can be shown as a widget through
 * QWidgetAction::requestWidget(QWidget*). It will return a combobox with the
 * given parent showing the projects to be selected.
 * \code
 * projectSelection = new ProjectSelection(parent, projectFilter);
 * projectSelection->setToolBarMode(KSelectAction::ComboBoxMode);
 * projectSelectionWidget = projectSelection->requestWidget(parentWidget);
 * layout->addWidget(projectSelectionWidget);
 * \endcode
 *
 * If the ProjectSelection is going to be added to a QToolBar (as it inherits
 * QAction), it can be shown instead as a button with "Project" label that pop
 * ups the added projects to be selected using
 * setToolBarMode(KSelectAction::ToolBarMode) with KSelectAction::MenuMode
 * parameter, and adding the ProjectSelection to the QToolBar.
 * \code
 * projectSelection = new ProjectSelection(parent, projectFilter);
 * projectSelection->setToolBarMode(KSelectAction::MenuMode);
 * toolBar->addAction(projectSelection);
 * \endcode
 */
class VERITAS_EXPORT ProjectSelection: public KSelectAction
{
Q_OBJECT
public:

    /**
     * Filters the projects that can be selected in a ProjectSelection.
     * Method canBeSelected must be implemented in subclasses.
     *
     * An IProjectFilter can offer an explanation about why it disables items.
     * This will be shown in the "What is this?" help of its ProjectSelection.
     */
    class IProjectFilter
    {
    public:

        /**
         * Create a new IProjectFilter with the given explanation.
         * If an explanation is set, KUIT "para" markup must be used. That is:
         * \code
         * IProjectFilter(i18nc("@info:whatsthis", "<para>The explanation.</para>"));
         * \endcode
         *
         * @param explanation The explanation about the IProjectFilter.
         */
        explicit IProjectFilter(const QString& explanation = QString());

        /**
         * Destroys this IProjectFilter.
         */
        virtual ~IProjectFilter();

        /**
         * Checks whether the given project can be selected or not.
         * This method must be implemented in subclasses.
         *
         * @param project The project to check.
         * @return True if the project can be selected, false otherwise.
         */
        virtual bool canBeSelected(KDevelop::IProject* project) = 0;

        /**
         * Returns the filter explanation.
         *
         * @return The filter explanation.
         */
        QString explanation() const;

    private:

        class IProjectFilterPrivate* d;

    };

    /**
     * Creates a new ProjectSelection with the given parent and IProjectFilter.
     * All the currently opened projects are added to the ProjectSelection.
     * The ProjectSelection takes ownership of the filter, so it is deleted when
     * needed.
     *
     * @param parent The parent object, defaults to null pointer.
     * @param projectFilter The IProjectFilter to use, defaults to null pointer.
     */
    explicit ProjectSelection(QObject* parent = 0, IProjectFilter* projectFilter = 0);

    /**
     * Destroys this ProjectSelection.
     * The IProjectFilter is deleted, if any.
     */
    virtual ~ProjectSelection();

    /**
     * Returns a list with all the currently opened projects.
     * Note that all the projects are completely opened and their model
     * populated.
     *
     * @return A list with all the currently opened projects.
     */
    QList<KDevelop::IProject*> projects() const;

    /**
     * Selects the given project.
     * The selected project can be cleared using a null pointer.
     *
     * The current selection isn't changed if the project wasn't added to this
     * ProjectSelection or if the filter excludes it from being selected.
     *
     * Note that projectSelected(KDevelop::IProject*) signal isn't emitted when
     * the project is selected using this method.
     *
     * @param project The project to select.
     * @return True if the project was selected, false otherwise.
     */
    bool selectProject(KDevelop::IProject* project);

Q_SIGNALS:

    /**
     * This signal is emitted when a project is selected.
     * The selection can be made by the user, or it can be a deselection caused
     * by closing a project or updating its configuration, but it is not emitted
     * when selectProject(KDevelop::IProject*) is called.
     *
     * @param project The project selected, or a null pointer if the current
     *        project was deselected.
     */
    void projectSelected(KDevelop::IProject* project);

private:

    class ProjectSelectionPrivate* d;

private Q_SLOTS:

    /**
     * Adds a new checkable action for the given project.
     * The action has the same name as the project. If there is a filter set and
     * the project can not be selected, the action is disabled.
     *
     * @param project The project to add.
     */
    void addProject(KDevelop::IProject* project);

    /**
     * Updates the action associated with the given project.
     * If there is a filter and the action can not be selected, the action is
     * disabled. Otherwise, the action is enabled.
     *
     * If the currently selected action gets disabled, the selection is cleared
     * and projectSelected(KDevelop::IProject*) with a null pointer is emitted.
     *
     * @param project The project which configuration was updated.
     */
    void updateProject(KDevelop::IProject* project);

    /**
     * Removes the action associated with the given project.
     * If the currently selected action is removed, the selection is cleared and
     * projectSelected(KDevelop::IProject*) with a null pointer is emitted.
     *
     * @param project The project to remove.
     */
    void removeProject(KDevelop::IProject* project);

    /**
     * Selects the project for the given action.
     *
     * @param action The action selected by the user.
     */
    void selectAction(QAction* action);

};

}

#endif
