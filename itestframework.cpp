/*
 * This file is part of KDevelop
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

#include "itestframework.h"
#include "internal/toolviewdata.h"
#include "internal/itestframework_p.h"

using Veritas::ITestFramework;
using Veritas::ITestFrameworkPrivate;

ITestFramework::ITestFramework()
    : d(new ITestFrameworkPrivate())
{
    d->self = this;
    g_toolViewStore[this] = ToolViewData();
}

ITestFramework::~ITestFramework()
{
    delete d;
}

QWidget* ITestFramework::createConfigWidget()
{
    return 0;
}

KDevelop::ProjectConfigSkeleton* ITestFramework::configSkeleton(const QVariantList& args)
{
    Q_UNUSED(args);
    return 0;
}
