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

#ifndef VERITAS_VERBOSETOGGLE_H
#define VERITAS_VERBOSETOGGLE_H

#include <QAbstractButton>
#include <QPixmap>
#include <QModelIndex>

class QTimeLine;

namespace Veritas
{

/**
 * @brief Toggle button for spawning verbose test output
 *
 * The toggle button is visually invisible until it is displayed at least
 * for one second.
 *
 * @see VerboseManager
 * @todo extract common code with selectiontoggle
 */
class VerboseToggle : public QAbstractButton
{
    Q_OBJECT

public:
    explicit VerboseToggle(QWidget* parent);
    virtual ~VerboseToggle();
    virtual QSize sizeHint() const;

    /**
     * Resets the selection toggle so that it is hidden and stays
     * visually invisible for at least one second after it is shown again.
     */
    void reset();

    void setIndex(const QModelIndex& index);
    QModelIndex index();

public slots:
    virtual void setVisible(bool visible);

protected:
    virtual bool eventFilter(QObject* obj, QEvent* event);
    virtual void enterEvent(QEvent* event);
    virtual void leaveEvent(QEvent* event);
    virtual void paintEvent(QPaintEvent* event);

private slots:
    /**
     * Sets the alpha value for the fading animation and is
     * connected with m_fadingTimeLine.
     */
    void setFadingValue(int value);

    void setIconOverlay();
    void refreshIcon();

private:
    void startFading();
    void stopFading();

private:
    bool m_isHovered;
    int m_fadingValue;
    QPixmap m_icon;
    QTimeLine* m_fadingTimeLine;
    QModelIndex m_index;
};

}

#endif // VERITAS_VERBOSETOGGLE_H
