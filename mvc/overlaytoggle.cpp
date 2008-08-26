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

#include "overlaytoggle.h"
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

using Veritas::OverlayButton;

QModelIndex OverlayButton::index()
{
    return m_index;
}

void OverlayButton::setIndex(const QModelIndex& index)
{
    m_index = index;
}

void OverlayButton::reset()
{
    m_index = QModelIndex();
    hide();
}


void OverlayButton::setVisible(bool visible)
{
    QAbstractButton::setVisible(visible);

    stopFading();
    if (visible) {
        startFading();
    }

}

OverlayButton::OverlayButton(QWidget* parent) : QAbstractButton(parent), m_fadingValue(0), m_fadingTimeLine(0),
    m_isHovered(false),
    m_icon()
{}


void OverlayButton::setFadingValue(int value)
{
    m_fadingValue = value;
    if (m_fadingValue >= 255) {
        Q_ASSERT(m_fadingTimeLine != 0);
        m_fadingTimeLine->stop();
    }
    update();
}

void OverlayButton::startFading()
{
    Q_ASSERT(m_fadingTimeLine == 0);

    const bool animate = KGlobalSettings::graphicEffectsLevel() & KGlobalSettings::SimpleAnimationEffects;
    const int duration = animate ? 600 : 1;

    m_fadingTimeLine = new QTimeLine(duration, this);
    connect(m_fadingTimeLine, SIGNAL(frameChanged(int)),
            this, SLOT(setFadingValue(int)));
    m_fadingTimeLine->setFrameRange(0, 255);
    m_fadingTimeLine->start();
    m_fadingValue = 0;
}

void OverlayButton::stopFading()
{
    if (m_fadingTimeLine != 0) {
        m_fadingTimeLine->stop();
        delete m_fadingTimeLine;
        m_fadingTimeLine = 0;
    }
    m_fadingValue = 0;
}

void OverlayButton::leaveEvent(QEvent* event)
{
    QAbstractButton::leaveEvent(event);
    m_isHovered = false;
    update();
}

void OverlayButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    // draw an alpha blended circle as background
    const QPalette& palette = parentWidget()->palette();

    const QBrush& backgroundBrush = palette.brush(QPalette::Normal, QPalette::Window);
    QColor background = backgroundBrush.color();
    background.setAlpha(m_fadingValue / 2);
    painter.setBrush(background);

    const QBrush& foregroundBrush = palette.brush(QPalette::Normal, QPalette::WindowText);
    QColor foreground = foregroundBrush.color();
    foreground.setAlpha(m_fadingValue / 4);
    painter.setPen(foreground);

    //painter.drawEllipse(0, 0, width(), height());

    // draw the icon overlay
    if (m_isHovered) {
        KIconEffect iconEffect;
        QPixmap activeIcon = iconEffect.apply(m_icon, KIconLoader::Desktop, KIconLoader::ActiveState);
        painter.drawPixmap(0, 0, activeIcon);
    } else {
        if (m_fadingValue < 255) {
            // apply an alpha mask respecting the fading value to the icon
            QPixmap icon = m_icon;
            QPixmap alphaMask(icon.width(), icon.height());
            const QColor color(m_fadingValue, m_fadingValue, m_fadingValue);
            alphaMask.fill(color);
            icon.setAlphaChannel(alphaMask);
            painter.drawPixmap(0, 0, icon);
        } else {
            // no fading is required
            painter.drawPixmap(0, 0, m_icon);
        }
    }
}


#include "overlaytoggle.moc"