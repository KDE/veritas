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

#include "verbosetoggle.h"

#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QTimer>
#include <QTimeLine>

#include <KDebug>
#include <KGlobalSettings>
#include <KIcon>
#include <KIconLoader>
#include <KIconEffect>
#include <KLocale>
#include "veritas/test.h"

using Veritas::Test;
using Veritas::VerboseToggle;
using Veritas::OverlayButton;

VerboseToggle::VerboseToggle(QWidget* parent) :
    OverlayButton(parent)
{
    setFocusPolicy(Qt::NoFocus);
    parent->installEventFilter(this);
    resize(sizeHint());
    setIconOverlay();
    connect(this, SIGNAL(toggled(bool)),
            this, SLOT(setIconOverlay()));
    connect(KGlobalSettings::self(), SIGNAL(iconChanged(int)),
            this, SLOT(refreshIcon()));
    setToolTip(i18nc("@info:tooltip", "Verbose Output"));
}

VerboseToggle::~VerboseToggle()
{
}

QSize VerboseToggle::sizeHint() const
{
    return QSize(16, 16);
}

bool VerboseToggle::eventFilter(QObject* obj, QEvent* event)
{
    if ((obj == parent()) && (event->type() == QEvent::Leave)) {
        hide();
    }
    return QAbstractButton::eventFilter(obj, event);
}

void VerboseToggle::enterEvent(QEvent* event)
{
    QAbstractButton::enterEvent(event);

    // if the mouse cursor is above the toggle, display
    // it immediately without fading timer
    m_isHovered = true;
    if (m_fadingTimeLine != 0) {
        m_fadingTimeLine->stop();
    }
    m_fadingValue = 255;
    update();
}

bool VerboseToggle::shouldShow(Test* t)
{
    return t!=0 && t->needVerboseToggle();
}

void VerboseToggle::setIconOverlay()
{
    m_icon = KIconLoader::global()->loadIcon(
        "document-open",
        KIconLoader::NoGroup,
        KIconLoader::SizeSmall);
    update();
}

void VerboseToggle::refreshIcon()
{
    setIconOverlay();
}


#include "verbosetoggle.moc"
