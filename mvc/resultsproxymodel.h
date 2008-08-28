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

#ifndef VERITAS_RESULTSPROXYMODEL_H
#define VERITAS_RESULTSPROXYMODEL_H

#include "veritas/mvc/proxymodelcommon.h"
#include "veritas/testresult.h"
#include "../veritasexport.h"

#include <QList>
#include <QSortFilterProxyModel>

namespace Veritas
{

class Test;

class ResultsModel;

/*!
 * \brief The ResultsProxyModel class provides support for sorting and
 *        filtering data passed between ResultsModel and a view.
 *
 * When the model is set to inactive no data is returned at all. When a
 * column of the model is disabled, no data is returned for that column.
 *
 * Row filtering is based on the result for a particular row. Rows with
 * a Veritas::RunException result are never filtered out.
 *
 * \sa \ref views
 */
class VERITAS_EXPORT ResultsProxyModel : public QSortFilterProxyModel,
                          public ProxyModelCommon
{
    Q_OBJECT

public: // Operations

    /*!
     * Constructs a results proxy model with the given \a parent and
     * \a filter.
     */
    explicit ResultsProxyModel(QObject* parent, int filter = Veritas::AllStates);

    /*!
     * Destroys this results proxy model.
     */
    ~ResultsProxyModel();

    /*!
     * Returns the data stored under the given \a role for the item
     * referred to by \a index.
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    /*!
     * Returns the active filter. Is a combination of OR'ed
     * Veritas::RunnerResult values.
     */
    int filter() const;

    /*!
     * Sets the \a filter for the model. Is a combination of OR'ed
     * Veritas::RunnerResult values. Result types defined in the
     * filter are included in the model.
     */
    void setFilter(int filter);
    void setTestFilter(const QList<Test*>&);
    void resetTestFilter();

protected: // Operations

    /*!
     * Returns true if the value in the item in the row indicated by
     * \a source_row should be included in the model. \a source_parent
     * is ignored.
     */
    bool filterAcceptsRow(int source_row,
                          const QModelIndex& source_parent) const;

private: // Operations

    /*!
     * Returns the model that contains the data that is available
     * through the proxy model.
     */
    ResultsModel* model() const;

    // Copy and assignment not supported.
    ResultsProxyModel(const ResultsProxyModel&);
    ResultsProxyModel& operator=(const ResultsProxyModel&);

private: // Attributes
    int m_filter;
    QList<Test*> m_testFilter;
};

} // namespace

#endif // VERITAS_RESULTSPROXYMODEL_H
