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

#ifndef VERITAS_RESULTSMODEL_H
#define VERITAS_RESULTSMODEL_H

#include <QAbstractListModel>
#include <QStringList>
#include "../veritasexport.h"

namespace Veritas
{

class Test;

/*!
 * \brief The ResultsModel class maintains results data in a
 *        non-hierarchical list.
 *
 * This class stores the results of runner items. Actually not the
 * result values of type Veritas::RunnerResult but runner item
 * indexes are kept.
 *
 * \sa \ref results_model and \ref runner_item_index
 */
class VERITAS_EXPORT ResultsModel : public QAbstractListModel
{
Q_OBJECT
public: // Operations

    /*!
     * Constructs a results model with the given \a headerData and
     * \a parent.
     */
    explicit ResultsModel(const QStringList& headerData, QObject* parent = 0);

    /*!
     * Destroys this results model.
     */
    virtual ~ResultsModel();

    /*!
     * Returns the data stored under the given \a role for the item
     * referred to by \a index.
     */
    QVariant data(const QModelIndex& index, int role) const;

    /*!
     * Returns the data for the given \a role and \a section in the
     * header with the specified \a orientation.
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    /*!
     * Returns the number of rows. This corresponds to the number of
     * available results. \a parent is ignored.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     * Returns the number of columns. \a parent is ignored.
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex& parent = QModelIndex()) const;

    /*!
     * Returns the result located at \a row. If no entry is at the
     * given position Veritas::NoResult is returned.
     */
    int result(int row) const;

    /*!
     * Returns the runner item index related to the results model
     * \a index.
     */
    QModelIndex mapToTestIndex(const QModelIndex& index) const;

    /*!
     * Returns the results model index related to the runner item
     * index \a testItemIndex.
     */
    QModelIndex mapFromTestIndex(const QModelIndex& testItemIndex) const;

    /*!
     * Concatenates mapToTestIndex and itemFromIndex
     */
    Test* testFromIndex(const QModelIndex& i) const;

    /*!
     * Trigger a model update
     */
    void changed();

public slots:

    /*!
     * Adds \a testItemIndex at the end of the results list. Increases
     * the number of rows by one.
     */
    void addResult(const QModelIndex& testItemIndex);

    /*!
     * Removes all result entries. Forces attached views to update.
     */
    void clear();

private: // Operations

    /*!
     * Returns the runner item referred to by \a testItemIndex.
     */
    Test* itemFromIndex(const QModelIndex& testItemIndex) const;

    // Copy and assignment not supported.
    ResultsModel(const ResultsModel&);
    ResultsModel& operator=(const ResultsModel&);

private: // Attributes
    QStringList m_headerData;
    QList<QPersistentModelIndex> m_testItemIndexes;
    typedef QMap<qint64, int> TestMap;
    TestMap m_testItemMap;
};

} // namespace

#endif // VERITAS_RESULTSMODEL_H
