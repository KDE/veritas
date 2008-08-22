/***************************************************************************
 *   Copyright (C) 2008 by Peter Penz <peter.penz@gmx.at>                  *
 *             modified by Manuel Breugelmans <mbr.nxi@gmail.com>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#include "veritas/mvc/selectionmanager.h"

#include "veritas/test.h"
#include "veritas/utils.h"

#include "veritas/mvc/runnermodel.h"
#include "veritas/mvc/selectiontoggle.h"

#include <KIconEffect>

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAbstractProxyModel>
#include <QModelIndex>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QTimeLine>

using Veritas::SelectionManager;
using Veritas::SelectionToggle;
using Veritas::Test;
using Veritas::RunnerModel;
using Veritas::OverlayManager;

SelectionManager::SelectionManager(QAbstractItemView* parent) :
    OverlayManager(parent)
{}

void SelectionManager::setButton(OverlayButton* button)
{
    connect(button, SIGNAL(clicked(bool)),
            SLOT(setItemSelected(bool)));
    OverlayManager::setButton(button);
}

SelectionManager::~SelectionManager()
{}

void SelectionManager::setItemSelected(bool selected)
{
    const QModelIndex index = button()->index();
    if (index.isValid()) {
        index2Test(index)->setSelected(selected);
        QAbstractProxyModel* proxyModel = static_cast<QAbstractProxyModel*>(view()->model());
        RunnerModel* runnerModel = static_cast<RunnerModel*>(proxyModel->sourceModel());
        runnerModel->updateView(proxyModel->mapToSource(index));
        view()->viewport()->update();
    }
    emit selectionChanged();
}

#include "selectionmanager.moc"
